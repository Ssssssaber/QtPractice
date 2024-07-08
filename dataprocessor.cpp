#include "dataprocessor.h"
#include <QApplication>
#include <QDir>

QQueue<QString> DataProcessor::dataQueue;

DataProcessor::DataProcessor()
{
    dataMap["A"] = 1;
    dataMap["G"] = 2;
    dataMap["M"] = 3;
    dataMap["p1"] = 4;

    CircuitDataReceiver::connectCircuit();
    QString ans = CircuitDataReceiver::handleConfigParams('A', 1, 1, 1);
    if (ans != "")
        qDebug().noquote() << ans;
    this->setConfig();
    //emit signalStopConfigExec();
    //CircuitDataReceiver::disconnectCircuit();



    connect(this, &DataProcessor::signalLossDetected, &DataProcessor::slotOnPackageLoss);


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

void DataProcessor::setConfig()
{
    cdrworker = new CDRWorker();
    thread = new QThread();
    cdrworker->moveToThread(thread);

    connect(thread, SIGNAL(started()), cdrworker, SLOT(process()));
    connect(cdrworker, SIGNAL(finished(int)), thread, SLOT(quit()));
    connect(cdrworker, SIGNAL(finished(int)), this, SLOT(slotConfigCompleted(int)));
    connect(this, SIGNAL(signalStopConfigExec()), cdrworker, SLOT(stop()));
    connect(cdrworker, SIGNAL(finished(int)), cdrworker, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    thread->start();
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
    if (lastReceivedId + 1 == data.id)
    {
        QString message =  QString("no packages lost");
        emit signalLossDetected(message);
    }
    else
    {
        QString message =  QString("packages from %1 to %2 were lost").arg(lastReceivedId).arg(data.id);
        emit signalLossDetected(message);
    }
    lastReceivedId = data.id;

    return data;
}

void DataProcessor::slotOnPackageLoss(QString message)
{
    qDebug() << message;
}

void DataProcessor::slotDataFromDataReceiver(QString data)
{
    processLine(data);
}

void DataProcessor::slotConfigCompleted(int r)
{
    if ( LIBNII_SUCCESS != r )
    {
        QString err = QString("failed to set parameters: %1").arg(libnii_strerror(r));
        emit signalConfigError(err);
        qDebug().noquote() << err;
    }
}

void DataProcessor::receiveDataFromDataReceiver(QString data)
{
    dataQueue.enqueue(data);
}

void DataProcessor::readData()
{
    if(DataProcessor::dataQueue.isEmpty())
        return;
    processLine(DataProcessor::dataQueue.dequeue());
}
