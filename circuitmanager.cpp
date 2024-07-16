#include "circuitmanager.h"

#include "circuitdatareceiver.h"
#include "dataprocessor.h"
#include <QApplication>
#include <QDir>
#include <QtMath>

QQueue<QString> CircuitManager::errorQueue;

CircuitManager::CircuitManager()
{
    CircuitDataReceiver::connectCircuit();

    QTimer *cleanupTimer = new QTimer();
    cleanupTimer->setSingleShot(false);
    cleanupTimer->setInterval(2000);
    connect(cleanupTimer, &QTimer::timeout, this, &CircuitManager::slotCleanup);

    cleanupTimer->start();

    QTimer *deltaTimer = new QTimer();
    deltaTimer->setSingleShot(false);
    deltaTimer->setInterval(3000);
    connect(deltaTimer, &QTimer::timeout, this, &CircuitManager::slotUpdateDeltaTime);
    deltaTimer->start();

    QTimer *errorTimer = new QTimer();
    errorTimer->setSingleShot(false);
    errorTimer->setInterval(1000);
    connect(errorTimer, &QTimer::timeout, this, &CircuitManager::slotReadError);
    errorTimer->start();

    elapsedTimer = new QElapsedTimer();
    if (noCircuit)
    {
        QTimer *dataTimer = new QTimer();
        dataTimer->setInterval(10);
        connect(dataTimer, &QTimer::timeout, this, &CircuitManager::slotGenerateFakeData);
        elapsedTimer->start();
        dataTimer->start();
    }
}

void CircuitManager::setConfig()
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

int CircuitManager::getWindowSize()
{
    return windowSize;
}

bool CircuitManager::isWindowEnabled()
{
    return windowEnabled;
}

fullConfig CircuitManager::getFullConfig()
{
    return currentConfig;
}

std::vector<xyzCircuitData> CircuitManager::getData(char group)
{
    switch (group) {
    case 'A':
        return aData;
    case 'G':
        return gData;
    case 'M':
        return mData;
    default:
        return std::vector<xyzCircuitData>();
    }
}

void CircuitManager::addData(xyzCircuitData data)
{
    switch (data.group) {
    case 'A':
        aData.push_back(data);
        receivedMap['A'] += 1;
        break;
    case 'G':
        gData.push_back(data);
        receivedMap['G'] += 1;
        break;
    case 'M':
        mData.push_back(data);
        receivedMap['M'] += 1;
        break;
    default:
        break;
    }
}

void CircuitManager::slotConfigCompleted(int r)
{
    if (r == LIBNII_SUCCESS)
    {
        currentConfig = newConfig;
    }
}

void CircuitManager::slotWindowSizeChanged(int newSize)
{
    windowSize = newSize;
}

void CircuitManager::slotTimeToCleanChanged(int newTime)
{
    dataLifespanInSeconds = newTime;
}

void CircuitManager::slotConfigReceived(QList<cConfig> configsReceived)
{
    fullConfig conf = defaultConfig;
    conf = setConfigParamsFromList(configsReceived);
    if (!CircuitDataReceiver::checkForValidConfigParams(conf))
    {
        setConfig();
        newConfig = conf;
    }
}

CircuitManager::~CircuitManager()
{
    if (!CircuitDataReceiver::checkForValidConfigParams(defaultConfig))
    {
        setConfig();
    }
    CircuitDataReceiver::disconnectCircuit();
}

void CircuitManager::slotReadError()
{
    if(errorQueue.isEmpty())
        return;

    emit signalSendCircuitMessage(errorQueue.dequeue());
}

void CircuitManager::slotGenerateFakeData()
{

    char group;
    switch ((static_cast<int>(prng1.bounded(0, 3)))) {
    case 1:
        group = 'A';
        break;
    case 2:
        group = 'G';
        break;
    case 3:
        group = 'M';
        break;
    default:
        break;
    }

    xyzCircuitData data = {
        .group = group,
        .id = static_cast<int>(prng1.bounded(0, 256)),
        .x = static_cast<float>(prng1.bounded(-256, 256)),
        .y = static_cast<float>(prng1.bounded(-256, 256)),
        .z = static_cast<float>(prng1.bounded(-256, 256)),
        .timestamp = static_cast<float>(elapsedTimer->elapsed()),
    };
    DataProcessor::receiveDataFromDataReceiver(data);
}


fullConfig CircuitManager::setConfigParamsFromList(QList<cConfig> configs)
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

 void CircuitManager::receiveErrorFromDataReceiver(QString error)
{
    errorQueue.enqueue(error);
}


void CircuitManager::slotUpdateDeltaTime()
{
    xyzAnalysisResult deltaTimes = {
        .x = getAverageDeltaTime(aData, receivedMap['A']) * 1000,
        .y = getAverageDeltaTime(gData, receivedMap['G']) * 1000,
        .z = getAverageDeltaTime(mData, receivedMap['M']) * 1000,
    };

    receivedMap['A'] = 0;
    receivedMap['G'] = 0;
    receivedMap['M'] = 0;

    emit signalUpdatedDeltaTime(deltaTimes);
}

void CircuitManager::slotCleanup()
{
    aData = cleanDataListToTime(aData, dataLifespanInSeconds);
    gData = cleanDataListToTime(gData, dataLifespanInSeconds);
    mData = cleanDataListToTime(mData, dataLifespanInSeconds);
}

std::vector<xyzCircuitData> CircuitManager::cleanDataListToTime(std::vector<xyzCircuitData> dataToClean, int timeInSeconds)
{
    if (dataToClean.empty()) return std::vector<xyzCircuitData>();
    if (dataToClean.size() > maxVectorSize)
    {
        dataToClean = (createListSlice(dataToClean, maxVectorSize));
    }

    for (int i = 0; i < dataToClean.size(); i++)
    {
        if (dataToClean.front().timestamp - dataToClean[i].timestamp <= timeInSeconds)
            break;
        dataToClean.erase(dataToClean.begin());
    }
    return dataToClean;
}

std::vector<xyzCircuitData> CircuitManager::createListSlice(std::vector<xyzCircuitData> dataList, int size)
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

float CircuitManager::getAverageDeltaTime(std::vector<xyzCircuitData> data, int amount)
{
    int size = data.size();
    if (amount <= 0 || amount > data.size()) return 0;

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
    // p7Trace->P7_DEBUG(moduleName, TM("Frequency %f for %c with %d amount"), average, data.first().group, amount);
    return average;
}

void CircuitManager::slotSetAnalysisActive(QString analysisType, bool active)
{
    if (analysisType == "window")
    {
        windowEnabled = active;
    }
}
