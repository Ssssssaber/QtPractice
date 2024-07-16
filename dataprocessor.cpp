#include "dataprocessor.h"
#include <QApplication>
#include <QDir>
#include <QtMath>

std::queue<xyzCircuitData> DataProcessor::currentVector;


DataProcessor::DataProcessor(int udpPort, QHostAddress clientAddress, CircuitManager *manager, QObject *parent)
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

    this->udpPort = udpPort;
    this->clientAddress = clientAddress;

    this->manager = manager;

    lostData['A'] = 0;
    lostData['G'] = 0;
    lostData['M'] = 0;

    lastReceived['A'] = 0;
    lastReceived['G'] = 0;
    lastReceived['M'] = 0;

    aMap[0] = 3.90625f * 1e-6 * 9.81f;
    aMap[1] = 7.8125f * 1e-6 * 9.81f;
    aMap[2] = 15.625f * 1e-6 * 9.81f;

    gMap[0] = 1.f / 96.f;
    gMap[1] = 1.f / 48.f;
    gMap[2] = 1.f / 24.f;
    gMap[3] = 1.f / 8.f;

    // CircuitDataReceiver::connectCircuit();

}


void DataProcessor::receiveDataFromDataReceiver(xyzCircuitData data)
{
    currentVector.push(data);
}

void DataProcessor::slotStart()
{
    windowWorker = new WindowWorker();
    udpDataSocket = new QUdpSocket();
    readData();
}



void DataProcessor::sendData(xyzCircuitData data)
{
    QByteArray baDatagram;
    QDataStream out(&baDatagram, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_12);
    QDateTime dt = QDateTime::currentDateTime();

    xyzCircuitData data1 = data;
    out << dt << data1.toString();

    udpDataSocket->writeDatagram(baDatagram, clientAddress, udpPort);
}

void DataProcessor::readData()
{
    while (1)
    {
        if (currentVector.empty()) continue;
        queueSize = currentVector.size();
        qDebug() << "KEKE " << queueSize;
        processReceivedData(currentVector.back());
        currentVector.pop();

        if (queueSize > maxQueueSize)
        {
            currentVector = std::queue<xyzCircuitData>();
        }
    }
}


void DataProcessor::processReceivedData(xyzCircuitData data)
{

    switch (data.group) {
    case 'A':
        addDataWithAnalysisCheck(transformXyzData(data, aMap[manager->getFullConfig().aRange]));
        break;
    case 'G':
        addDataWithAnalysisCheck(transformXyzData(data, gMap[manager->getFullConfig().gRange]));
        break;
    case 'M':
        addDataWithAnalysisCheck(transformXyzData(data, mConstant));
        break;
    default:
        break;
    }
}

xyzCircuitData DataProcessor::transformXyzData(xyzCircuitData data, float transitionConst)
{
    if (data.group == 'A')
    {
        qDebug() << "before: " << data.toString();
    }

    data.x *= transitionConst;
    data.y *= transitionConst;
    data.z *= transitionConst;
    data.timestamp /= timeConstant;
    if (data.group == 'A')
    {
        qDebug() << "after: " << data.toString();
    }

    checkLost(data);

    return data;
}


void DataProcessor::checkLost(xyzCircuitData data)
{
    QString message;
    if (lastReceived[data.group] + 1 == data.id)
    {
        message =  QString("no packages lost");
        p7Trace->P7_TRACE(moduleName, TM("%s"), message.toStdString().data());
    }
    else
    {
        message =  QString("lost: %1 to %2").arg(lastReceived[data.group]).arg(data.id);

        lostData[data.group] += qFabs(lastReceived[data.group] - data.id);

        p7Trace->P7_CRITICAL(moduleName, TM("Lost %c : %ld"), data.group, lostData[data.group]);
        p7Trace->P7_WARNING(moduleName, TM("%s"), message.toStdString().data());
    }


    p7Trace->P7_TRACE(moduleName, TM("Data received %s"), data.toString().toStdString().data());
    lastReceived[data.group] = data.id;
}

void DataProcessor::addDataWithAnalysisCheck(xyzCircuitData newData)
{
    manager->addData(newData);

    if (manager->isWindowEnabled())
    {
        qDebug() << "window";
        sendData(windowWorker->doWork(createListSlice(manager->getData(newData.group), manager->getWindowSize())));
    }
    else
    {
        sendData(newData);
    }
}

std::vector<xyzCircuitData> DataProcessor::createListSlice(std::vector<xyzCircuitData> dataList, int size)
{
    std::vector<xyzCircuitData> listSlice = std::vector<xyzCircuitData>();

    if (size > dataList.size())
    {
        return dataList;
    }
    for (int i = dataList.size() - size;  i < dataList.size(); i++)
    {
        xyzCircuitData data = dataList[i];
        listSlice.push_back(data);
    }


    return listSlice;
}

