#include "dataprocessor.h"
#include "circuitdatareceiver.h"
#include <QApplication>
#include <QDir>

QQueue<QString> DataProcessor::dataQueue;

DataProcessor::DataProcessor()
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
    dataMap["A"] = 1;
    dataMap["G"] = 2;
    dataMap["M"] = 3;
    dataMap["p1"] = 4;

    CircuitDataReceiver *cdr = new CircuitDataReceiver();


    readTimer = new QTimer(this);
    readTimer->setSingleShot(false);
    readTimer->setInterval(0);
    connect(readTimer, &QTimer::timeout, this, &DataProcessor::readData);

    readTimer->start();

    fileTimer = new QTimer(this);
    fileTimer->setSingleShot(false);
    fileTimer->setInterval(0);
    connect(fileTimer, &QTimer::timeout, this, &DataProcessor::readLine);

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

void DataProcessor::processLine(QString line)
{

    QString processedLine = line.simplified();
    emit signalLineReceived(processedLine);
    QList<QString> tokens = line.simplified().split(' ');

    QString dataSource = tokens.data()[0];
    switch (dataMap[dataSource]) {
    case 1:
        emit signalLineProcessed(stringDataToStruct(tokens, aConstant));
        break;
    case 2:
        emit signalLineProcessed(stringDataToStruct(tokens, gConstant));
        break;
    case 3:
        emit signalLineProcessed(stringDataToStruct(tokens, mConstant));
        break;
    case 4:
        break;
    default:
        break;
    }
}


xyzCircuitData DataProcessor::stringDataToStruct(QList<QString> tokens, float transitionConst)
{

    xyzCircuitData data;
    data.group = tokens[0];
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


void DataProcessor::slotDataFromDataReceiver(QString data)
{
    processLine(data);
}

void DataProcessor::slotConfigReceived(cConfig config)
{
    qDebug() << "received config: " << config.toString();
}

void DataProcessor::receiveDataFromDataReceiver(QString data)
{
    dataQueue.enqueue(data);
}

void DataProcessor::readData()
{
    // fix queue
    if(DataProcessor::dataQueue.isEmpty())
        return;
    processLine(DataProcessor::dataQueue.dequeue());
}
