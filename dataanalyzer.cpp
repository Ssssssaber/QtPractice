#include "dataanalyzer.h"
// Q_DECLARE_METATYPE(QList<xyzCircuitData>)

DataAnalyzer::DataAnalyzer(DataProcessor *dataProcessor)
{
    // qRegisterMetaType<uint16_t>("QList<xyzCircuitData>");
    this->dataProcessor = dataProcessor;
    windowWorkerController = new XyzWorkerController(WorkerTypes::WindowWorker);
    connect(dataProcessor, &DataProcessor::signalLineProcessed, this, &DataAnalyzer::slotInfoReceived);
    connect(windowWorkerController, &XyzWorkerController::signalResultReady, this, &DataAnalyzer::slotResultReceived);

    cleanupTimer = new QTimer(this);
    cleanupTimer->setSingleShot(false);
    cleanupTimer->setInterval(timeToTimeout);
    connect(cleanupTimer, &QTimer::timeout, this, &DataAnalyzer::cleanup);

    cleanupTimer->start();
}

void DataAnalyzer::slotInfoReceived(xyzCircuitData data)
{
    if (data.group == "A")
    {
        addDataWithAnalysisCheck(&aData, data);
    }
    else if (data.group == "G")
    {
        addDataWithAnalysisCheck(&gData, data);
    }
    else if (data.group == "M")
    {
        addDataWithAnalysisCheck(&mData, data);
    }
    else
    {
        qDebug() << "WRONG DATA RECEIVED";
    }
}

void DataAnalyzer::slotWindowSizeChanged(int newSize)
{
    windowSize = newSize;
}

void DataAnalyzer::slotResultReceived(xyzAnalysisResult analysis)
{
    emit signalAnalysisReady(analysis);
}

void DataAnalyzer::cleanup()
{
    cleanDataListToTime(&aData, dataLifespanInSeconds);
    cleanDataListToTime(&gData, dataLifespanInSeconds);
    cleanDataListToTime(&mData, dataLifespanInSeconds);
}

void DataAnalyzer::cleanDataListToTime(QList<xyzCircuitData> *dataToClean, int timeInSeconds)
{
    int initial = dataToClean->length();
    foreach (xyzCircuitData data, dataToClean->toList())
    {
        if (dataToClean->last().timestamp - data.timestamp <= timeInSeconds)
            break;
        dataToClean->pop_back();
    }

    qDebug() << " before and after " << initial << dataToClean->length();
}

void DataAnalyzer::addDataWithAnalysisCheck(QList<xyzCircuitData>* dataList, xyzCircuitData newData)
{
    dataList->append(newData);
    // if (dataList->length() > windowSize)
    // {
        windowWorkerController->startOperating(createListSlice(dataList->toList(), windowSize));
    // }

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
