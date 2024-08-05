#include "circuitmanager.h"

#include "circuitdatareceiver.h"
#include "dataprocessor.h"
#include <QApplication>
#include <QDir>
#include <QtMath>

QQueue<QString> CircuitManager::errorQueue;

CircuitManager::CircuitManager(QObject *p) : QObject{p}
{
    p7Trace = P7_Get_Shared_Trace("ServerChannel");

    if (!p7Trace)
    {
        qDebug() << "circuit manager is not tracing";
    }
    else
    {
        p7Trace->Register_Module(TM("CMan"), &moduleName);
    }


    CircuitDataReceiver::connectCircuit();

    container = new DataContainer(this);

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

CircuitManager::~CircuitManager() {}

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


void CircuitManager::slotConfigCompleted(int r)
{
    if (r == LIBNII_SUCCESS)
    {
        currentConfig = newConfig;
        container->clearData();
        p7Trace->P7_TRACE(moduleName, TM("Config changed"));;
    }
}

void CircuitManager::slotWindowSizeChanged(int newSize)
{
    windowSize = newSize;
    p7Trace->P7_TRACE(moduleName, TM("Window changed to %d"), windowSize);;
}

void CircuitManager::slotTimeToCleanChanged(int newTime)
{
    dataLifespanInSeconds = newTime;
    p7Trace->P7_TRACE(moduleName, TM("Data lifespan changed to %d"), dataLifespanInSeconds);;
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

void CircuitManager::slotAddData(xyzCircuitData data)
{
    container->addData(data);
}

void CircuitManager::disconnectCircuit()
{
    if (!CircuitDataReceiver::checkForValidConfigParams(defaultConfig))
    {
        cdrworker = new CDRWorker();
        thread = new QThread();
        cdrworker->moveToThread(thread);

        connect(thread, SIGNAL(started()), cdrworker, SLOT(disconnectCircuit()));
        connect(cdrworker, SIGNAL(finished()), thread, SLOT(quit()));
        connect(cdrworker, SIGNAL(finished()), cdrworker, SLOT(deleteLater()));
        connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

        thread->start();
    }
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
        .x = getAverageDeltaTime(container->getData('A')),
        .y = getAverageDeltaTime(container->getData('G')),
        .z = getAverageDeltaTime(container->getData('M')),
    };

    emit signalUpdatedDeltaTime(deltaTimes);
}

float CircuitManager::getAverageDeltaTime(std::vector<xyzCircuitData> dataList)
{
    if (dataList.empty()) return 0.f;
    int size = dataList.size();
    float res = 0;

    std::vector<xyzCircuitData>::iterator it;
    xyzCircuitData previous = {};
    for (it = dataList.begin(); it != dataList.end(); it++)
    {
        if (it == dataList.begin())
        {
            previous = *it;
        }
        else
        {
            res += it->timestamp - previous.timestamp;
            previous = *it;
        }
    }

    res /= (float) size;
    float average = res / timeConstant;
    return average;
}

void CircuitManager::slotSetAnalysisActive(QString analysisType, bool active)
{
    if (analysisType == "window")
    {
        windowEnabled = active;
    }
}
