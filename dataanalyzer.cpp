#include "dataanalyzer.h"
// Q_DECLARE_METATYPE(QList<xyzCircuitData>)

DataAnalyzer::DataAnalyzer(DataProcessor *dataProcessor)
{
    p7Trace = P7_Get_Shared_Trace("ServerChannel");

    if (!p7Trace)
    {
        qDebug() << "data analyzer is not tracing";
    }
    else
    {
        p7Trace->Register_Module(TM("DAn"), &moduleName);
    }


    // qRegisterMetaType<uint16_t>("QList<xyzCircuitData>");
    this->dataProcessor = dataProcessor;
    // windowWorkerController = new XyzWorkerController(WorkerTypes::WindowWorker);
    windowWorkerController = new XyzWorkerController(WorkerTypes::FilterWorker);
    connect(dataProcessor, &DataProcessor::signalLineProcessed, this, &DataAnalyzer::slotInfoReceived);
    connect(dataProcessor, &DataProcessor::signalSendDtOption , windowWorkerController, &XyzWorkerController::slotHandleDtOption);
    connect(windowWorkerController, &XyzWorkerController::signalResultReady, this, &DataAnalyzer::slotResultReceived);

    cleanupTimer = new QTimer(this);
    cleanupTimer->setSingleShot(false);
    cleanupTimer->setInterval(timeToTimeout);
    connect(cleanupTimer, &QTimer::timeout, this, &DataAnalyzer::slotCleanup);

    cleanupTimer->start();

    QTimer *deltaTimer = new QTimer(this);
    deltaTimer->setSingleShot(false);
    deltaTimer->setInterval(3000);
    connect(deltaTimer, &QTimer::timeout, this, &DataAnalyzer::slotUpdateDeltaTime);
    deltaTimer->start();
}

void DataAnalyzer::slotInfoReceived(xyzCircuitData data)
{
    if (data.group == 'A')
    {
        aReceived += 1;
        addDataWithAnalysisCheck(&aData, data);
    }
    else if (data.group == 'G')
    {
        gReceived += 1;
        addDataWithAnalysisCheck(&gData, data);
    }
    else if (data.group == 'M')
    {
        mReceived += 1;
        addDataWithAnalysisCheck(&mData, data);
    }
    else
    {
        p7Trace->P7_ERROR(moduleName, TM("Wrong data on data analyzer"));
    }
}

void DataAnalyzer::slotWindowSizeChanged(int newSize)
{
    windowSize = newSize;
}

void DataAnalyzer::slotTimeToCleanChanged(int newTime)
{
    dataLifespanInSeconds = newTime;
}

void DataAnalyzer::slotAnalysisToggled(QString analysisType)
{
    if (analysisType == "window")
    {
        windowWorkerController->isEnabled = !windowWorkerController->isEnabled;
    }
    else
    {
        p7Trace->P7_ERROR(moduleName, TM("Data analysis method not Implemented"));
    }
}

void DataAnalyzer::slotResultReceived(xyzAnalysisResult analysis)
{
    emit signalAnalysisReady(analysis);
    p7Trace->P7_TRACE(moduleName, TM("Analysis ready: %s"), analysis.toString().toStdString().data());
}

void DataAnalyzer::slotUpdateDeltaTime()
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

void DataAnalyzer::slotCleanup()
{
    cleanDataListToTime(&aData, dataLifespanInSeconds);
    cleanDataListToTime(&gData, dataLifespanInSeconds);
    cleanDataListToTime(&mData, dataLifespanInSeconds);
}

void DataAnalyzer::cleanDataListToTime(QList<xyzCircuitData> *dataToClean, int timeInSeconds)
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

float DataAnalyzer::getAverageDeltaTime(QList<xyzCircuitData> data, int amount)
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

void DataAnalyzer::addDataWithAnalysisCheck(QList<xyzCircuitData>* dataList, xyzCircuitData newData)
{
    dataList->append(newData);
    inCount += 1;
    if (inCount == analysisFrequency)
    {
        if (windowWorkerController->isEnabled)
            windowWorkerController->startOperating(createListSlice(dataList->toList(), windowSize));
        inCount = 0;
    }
    else if (inCount > analysisFrequency)
    {
        p7Trace->P7_CRITICAL(moduleName, TM("ANALYSIS FREQUENCY ERROR"));
        inCount = 0;
    }
}

QList<xyzCircuitData> DataAnalyzer::createListSlice(QList<xyzCircuitData> dataList, int size)
{
    QList<xyzCircuitData> listSlice = QList<xyzCircuitData>();

    if (size > dataList.length())
    {
        return dataList;
    }
    for (int i = dataList.length() - 1; i > dataList.length() - 1 - size; i--)
    {
        xyzCircuitData data = dataList[i];
        listSlice.append(data);
    }

    return listSlice;
}
