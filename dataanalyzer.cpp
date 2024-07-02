#include "dataanalyzer.h"

DataAnalyzer::DataAnalyzer(DataProcessor *dataProcessor)
{
    this->dataProcessor = dataProcessor;
    windowWorkerController = new XyzWorkerController(WorkerTypes::WindowWorker);
    connect(dataProcessor, &DataProcessor::signalLineProcessed, this, &DataAnalyzer::slotInfoReceived);
}

void DataAnalyzer::slotInfoReceived(xyzCircuitData data)
{
    if (data.group == "A")
    {
        AddDataWithAnalysisCheck(&aData, data);
    }
    else if (data.group == "G")
    {
        AddDataWithAnalysisCheck(&gData, data);
    }
    else if (data.group == "M")
    {
        AddDataWithAnalysisCheck(&mData, data);
    }
    else
    {
        qDebug() << "WRONG DATA RECEIVED";
    }
}

void DataAnalyzer::AddDataWithAnalysisCheck(QList<xyzCircuitData>* dataList, xyzCircuitData newData)
{
    dataList->append(newData);
    if (dataList->length() > windowSize)
    {
        windowWorkerController->startOperating(dataList->toList());
    }

}
