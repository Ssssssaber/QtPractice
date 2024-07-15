#include "dataprocessor.h"
#include "cdrworker.h"
#include "circuitdatareceiver.h"
#include <QApplication>
#include <QDir>
#include <QtMath>

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

    aMap[0] = 3.90625f * 1e-6 * 9.81f;
    aMap[1] = 7.8125f * 1e-6 * 9.81f;
    aMap[2] = 15.625f * 1e-6 * 9.81f;

    gMap[0] = 1.f / 96.f;
    gMap[1] = 1.f / 48.f;
    gMap[2] = 1.f / 24.f;
    gMap[3] = 1.f / 8.f;

    CircuitDataReceiver::connectCircuit();
    // setConfig();

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

    // fileTimer = new QTimer(this);
    // fileTimer->setSingleShot(false);
    // fileTimer->setInterval(0);
    // connect(fileTimer, &QTimer::timeout, this, &DataProcessor::readLine);

    // windowWorkerController = new XyzWorkerController(WorkerTypes::WindowWorker);
    windowWorker = new WindowWorker();
    // connect(windowWorkerController, &XyzWorkerController::signalResultReady, this, &DataP::slotResultReceived);

    cleanupTimer = new QTimer(this);
    cleanupTimer->setSingleShot(false);
    cleanupTimer->setInterval(timeToTimeout);
    connect(cleanupTimer, &QTimer::timeout, this, &DataProcessor::slotCleanup);

    cleanupTimer->start();

    QTimer *deltaTimer = new QTimer(this);
    deltaTimer->setSingleShot(false);
    deltaTimer->setInterval(3000);
    connect(deltaTimer, &QTimer::timeout, this, &DataProcessor::slotUpdateDeltaTime);
    deltaTimer->start();
}

DataProcessor::~DataProcessor()
{
    if (!CircuitDataReceiver::checkForValidConfigParams(defaultConfig))
    {
        setConfig();
    }
    CircuitDataReceiver::disconnectCircuit();

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

void DataProcessor::slotUpdateDeltaTime()
{
    xyzAnalysisResult deltaTimes = {
        .x = getAverageDeltaTime(aData, aReceived) * 1000,
        .y = getAverageDeltaTime(gData, gReceived) * 1000,
        .z = getAverageDeltaTime(mData, mReceived) * 1000,
    };

    aReceived = 0;
    gReceived = 0;
    mReceived = 0;

    emit signalUpdatedDeltaTime(deltaTimes);
}

void DataProcessor::slotCleanup()
{
    cleanDataListToTime(&aData, dataLifespanInSeconds);
    cleanDataListToTime(&gData, dataLifespanInSeconds);
    cleanDataListToTime(&mData, dataLifespanInSeconds);
}

void DataProcessor::cleanDataListToTime(QList<xyzCircuitData> *dataToClean, int timeInSeconds)
{
    if (dataToClean->isEmpty()) return;

    int initial = dataToClean->length();
    foreach (xyzCircuitData data, dataToClean->toList())
    {
        if (dataToClean->last().timestamp - data.timestamp <= timeInSeconds)
            break;
        dataToClean->pop_front();
    }

    // qDebug() << " before and after " << initial << dataToClean->length();
    p7Trace->P7_TRACE(moduleName, TM("Cleared %c analyzer arrays: from %d to %d"),
                      dataToClean->back().group,
                      initial, dataToClean->length());
}

float DataProcessor::getAverageDeltaTime(QList<xyzCircuitData> data, int amount)
{
    int size = data.length();
    if (amount <= 0 || amount > data.length()) return 0;

    // if (amount == 1) return 0;
    // float first = data.last().timestamp;
    // float second = data[data.length() - 2].timestamp;

    // float average = first - second;

    float average = 0;

    for(int i = size - amount; i < size - 1; i++)
    {
        average += data[i + 1].timestamp - data[i].timestamp;
    }
    average /= (float) amount;
    p7Trace->P7_DEBUG(moduleName, TM("Frequency %f for %c with %d amount"), average, data.first().group, amount);
    return average;
}

void DataProcessor::addDataWithAnalysisCheck(QList<xyzCircuitData> *dataList, xyzCircuitData newData)
{
    dataList->append(newData);
    inCount += 1;
    if (windowWorker->isEnabled)
    {
        qDebug() << "window";
        emit signalLineProcessed(windowWorker->doWork(createListSlice(dataList->toList(), windowSize)));
    }
    else
    {
        emit signalLineProcessed(newData);
    }
}

QList<xyzCircuitData> DataProcessor::createListSlice(QList<xyzCircuitData> dataList, int size)
{
    QList<xyzCircuitData> listSlice = QList<xyzCircuitData>();

    if (size > dataList.length())
    {
        return dataList;
    }
    for (int i = dataList.length() - size;  i < dataList.length(); i++)
    {
        xyzCircuitData data = dataList[i];
        listSlice.append(data);
    }


    return listSlice;
}

void DataProcessor::processReceivedData(xyzCircuitData data)
{
    try
    {
        switch (dataMap[data.group]) {
        case 1:
            addDataWithAnalysisCheck(&aData, transformXyzData(data, aMap[currentConfig.aRange]));
            break;
        case 2:
            addDataWithAnalysisCheck(&gData, transformXyzData(data, gMap[currentConfig.gRange]));
            break;
        case 3:
            addDataWithAnalysisCheck(&mData, transformXyzData(data, mConstant));
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


    QString message;
    if (lastReceivedId + 1 == data.id)
    {
        message =  QString("no packages lost");
        p7Trace->P7_TRACE(moduleName, TM("%s"), message.toStdString().data());
    }
    else
    {
        message =  QString("lost: %1 to %2").arg(lastReceivedId).arg(data.id);

        switch(data.group)
        {
        case 'A':
            aLost += qFabs(data.id - lastReceivedId);
            p7Trace->P7_CRITICAL(moduleName, TM("Lost %c : %ld"), data.group, aLost);
            break;
        case 'G':
            gLost += qFabs(data.id - lastReceivedId);
            p7Trace->P7_CRITICAL(moduleName, TM("Lost %c : %ld"), data.group, gLost);
            break;
        case 'M':
            mLost += qFabs(data.id - lastReceivedId);
            p7Trace->P7_CRITICAL(moduleName, TM("Lost %c : %ld"), data.group, mLost);
            break;
        }

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
        currentConfig = newConfig;
    }
}

void DataProcessor::slotConfigReceived(QList<cConfig> configsReceived)
{
    fullConfig conf = defaultConfig;
    conf = setConfigParamsFromList(configsReceived);
    if (!CircuitDataReceiver::checkForValidConfigParams(conf))
    {
        setConfig();
        newConfig = conf;
    }
}

void DataProcessor::slotWindowSizeChanged(int newSize)
{
    windowSize = newSize;
}

void DataProcessor::slotTimeToCleanChanged(int newTime)
{
    dataLifespanInSeconds = newTime;
}

void DataProcessor::slotSetAnalysisActive(QString analysisType, bool active)
{
    if (analysisType == "window")
    {
        windowWorker->isEnabled = active;
    }
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
    else if (currentData.group == 'A' ||
             currentData.group == 'G' ||
             currentData.group == 'M')
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

fullConfig DataProcessor::setConfigParamsFromList(QList<cConfig> configs)
{
    fullConfig full;

    foreach (cConfig config, configs)
    {
        if (config.type == "A")
        {
            full.aAvg = config.avg;
            full.aFreq = config.freq;
            full.aRange = config.range;
        }
        else if (config.type == "G")
        {
            full.gAvg = config.avg;
            full.gFreq = config.freq;
            full.gRange = config.range;
        }
        else if (config.type == "M")
        {
            full.mAvg = config.avg;
            full.mFreq = config.freq;
        }
    }

    return full;
}
