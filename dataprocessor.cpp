#include "dataprocessor.h"
#include "circuitdatareceiver.h"
#include <QApplication>
#include <QDir>

QQueue<xyzCircuitData> DataProcessor::dataQueue;
QQueue<QString> DataProcessor::errorQueue;
xyzCircuitData DataProcessor::currentData;

DataProcessor::DataProcessor(QObject *parent)
    : QObject{parent}
{
    p7Trace = P7_Get_Shared_Trace("ServerChannel");

    if (!p7Trace)
    {
        qDebug() << "data processor is not tracing";
    }
    else
    {
        p7Trace->Register_Module(TM("DProc"), &moduleName);
    }
    dataMap['A'] = 1;
    dataMap['G'] = 2;
    dataMap['M'] = 3;

    aMap[0] = 3.90625f * 1e-6 * 2.;
    aMap[1] = 7.8125f * 1e-6 * 2.;
    aMap[2] = 15.625f * 1e-6 * 2.;

    gMap[0] = 1.f / 96.f;
    gMap[1] = 1.f / 48.f;
    gMap[2] = 1.f / 24.f;
    gMap[3] = 1.f / 8.f;

    CircuitDataReceiver::connectCircuit();
    setConfig();

    dataTimer = new QTimer(this);
    dataTimer->setSingleShot(false);
    dataTimer->setInterval(0);
    connect(dataTimer, &QTimer::timeout, this, &DataProcessor::readData);
    dataTimer->start();

    errorTimer = new QTimer();
    errorTimer->setSingleShot(false);
    errorTimer->setInterval(0);
    connect(errorTimer, &QTimer::timeout, this, &DataProcessor::readError);
    errorTimer->start();

    fileTimer = new QTimer(this);
    fileTimer->setSingleShot(false);
    fileTimer->setInterval(0);
    connect(fileTimer, &QTimer::timeout, this, &DataProcessor::readLine);
}

DataProcessor::~DataProcessor()
{
    CircuitDataReceiver::disconnectCircuit();
}

void DataProcessor::readDataFromTestFile()
{
    QString appPath = QDir().absolutePath();
    qInfo() << appPath;

    QDir dir(appPath);

    bool ok;
    ok = dir.cdUp();
    assert(ok);
    ok = dir.cdUp();
    assert(ok);

    qInfo() << dir.path();
    auto keke = dir.path() + "/circuit_data/all_data.txt";
    file = new QFile(keke);
    if (!file->open(QIODevice::ReadOnly)) return;

    QString line = file->readLine().simplified();
    processLine(line);


    fileTimer->start();
}

void DataProcessor::readLine()
{
    if (file->canReadLine())
    {
        QString line = file->readLine().simplified();
        if (line != "")
        {
            processLine(line);
        }
    }
}

void DataProcessor::setConfig()
{
    cdrworker = new CDRWorker();
    thread = new QThread();
    cdrworker->moveToThread(thread);

    connect(thread, SIGNAL(started()), cdrworker, SLOT(process()));
    connect(cdrworker, SIGNAL(finished(int)), thread, SLOT(quit()));
    connect(cdrworker, SIGNAL(finished(int)), this, SLOT(slotConfigCompleted(int)));
    connect(cdrworker, SIGNAL(finished(int)), cdrworker, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    thread->start();
}

void DataProcessor::processLine(QString line)
{
    try
    {
        QString processedLine = line.simplified();
        emit signalLineReceived(processedLine);
        QList<QString> tokens = line.simplified().split(' ');

        char dataSource = tokens.data()[0].toStdString().c_str()[0];
        switch (dataMap[dataSource]) {
        case 1:
            emit signalLineProcessed(stringDataToStruct(tokens, aMap[currentAConfig.range]));
            break;
        case 2:
            emit signalLineProcessed(stringDataToStruct(tokens, gMap[currentGConfig.range]));
            break;
        case 3:
            emit signalLineProcessed(stringDataToStruct(tokens, mConstant));
            break;
        default:
            break;
        }
    }
    catch (const std::exception& ex) {
        p7Trace->P7_CRITICAL(moduleName, TM("&s"), ex.what());
    }
    catch (...) {
        p7Trace->P7_CRITICAL(moduleName, TM("Unhandled exception in data processor"));
    }
}

void DataProcessor::processReceivedData(xyzCircuitData data)
{
    try
    {
        switch (dataMap[data.group]) {
        case 1:
            emit signalLineProcessed(transformXyzData(data, aMap[currentAConfig.range]));
            break;
        case 2:
            emit signalLineProcessed(transformXyzData(data, gMap[currentGConfig.range]));
            break;
        case 3:
            emit signalLineProcessed(transformXyzData(data, mConstant));
            break;
        case 4:
            break;
        default:
            break;
        }
    }
    catch (const std::exception& ex) {
        p7Trace->P7_CRITICAL(moduleName, TM("&s"), ex.what());
    }
    catch (...) {
        p7Trace->P7_CRITICAL(moduleName, TM("Unhandled exception in data processor"));
    }
}

xyzCircuitData DataProcessor::stringDataToStruct(QList<QString> tokens, float transitionConst)
{
    xyzCircuitData data;
    data.group = tokens[0].toStdString()[0];
    data.id = tokens[1].toInt();
    data.x = tokens[2].toInt() * transitionConst;
    data.y = tokens[3].toInt() * transitionConst;
    data.z = tokens[4].toInt() * transitionConst;
    data.timestamp = (tokens[5].toLong() / timeConstant);

    QString message;
    if (lastReceivedId + 1 == data.id)
    {
        message =  QString("no packages lost");
        p7Trace->P7_TRACE(moduleName, TM("%s"), message.toStdString().data());
    }
    else
    {
        message =  QString("lost: %1 to %2").arg(lastReceivedId).arg(data.id);
        p7Trace->P7_WARNING(moduleName, TM("%s"), message.toStdString().data());
    }

    p7Trace->P7_TRACE(moduleName, TM("Data received %s"), data.toString().toStdString().data());
    lastReceivedId = data.id;

    return data;
}

xyzCircuitData DataProcessor::transformXyzData(xyzCircuitData data, float transitionConst)
{    
    data.x *= transitionConst;
    data.y *= transitionConst;
    data.z *= transitionConst;
    data.timestamp /= timeConstant;
    qDebug() << data.group << data.x << data.y << data.z;

    QString message;
    if (lastReceivedId + 1 == data.id)
    {
        message =  QString("no packages lost");
        p7Trace->P7_TRACE(moduleName, TM("%s"), message.toStdString().data());
    }
    else
    {
        message =  QString("lost: %1 to %2").arg(lastReceivedId).arg(data.id);
        p7Trace->P7_WARNING(moduleName, TM("%s"), message.toStdString().data());
    }

    p7Trace->P7_TRACE(moduleName, TM("Data received %s"), data.toString().toStdString().data());
    lastReceivedId = data.id;

    return data;
}

void DataProcessor::slotConfigCompleted(int r)
{
    if (r == LIBNII_SUCCESS)
    {
        currentAConfig = newAConfig;
        currentGConfig = newGConfig;
        currentMConfig = newMConfig;
    }
}

void DataProcessor::slotConfigReceived(cConfig config)
{
    //if(!CircuitDataReceiver::handleConfigParams(???)
    // {
    //     if (config.type == "A")
    //     {
    //         newAConfig = config;
    //     }
    //     else if (config.type == "G")
    //     {
    //         newGConfig = config;
    //     }
    //     else if (config.type == "M")
    //     {
    //         newMConfig = config;
    //     }
    //     setConfig();
    // }

}

void DataProcessor::receiveDataFromDataReceiver(xyzCircuitData data)
{
    currentData = data;
}

void DataProcessor::readData()
{
    if (currentData.group == 'N')
    {
        return;
    }
    else
    {
        processReceivedData(currentData);
        currentData.group = 'N';
    }
}

void DataProcessor::receiveErrorFromDataReceiver(QString error)
{
    errorQueue.enqueue(error);
}

void DataProcessor::readError()
{
    if(errorQueue.isEmpty())
        return;

    emit signalSendCircuitMessage(errorQueue.dequeue());
}
