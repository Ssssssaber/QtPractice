#ifndef DATAANALYZER_H
#define DATAANALYZER_H

#include "dataprocessor.h"
#include "xyzcircuitdata.h"
#include "xyzworkercontroller.h"
#include <QtCore>

class DataAnalyzer : public QObject
{
    Q_OBJECT
private:
    int windowSize = 2;
    DataProcessor *dataProcessor;
    XyzWorkerController *windowWorkerController;
    QList<xyzCircuitData> aData;
    QList<xyzCircuitData> gData;
    QList<xyzCircuitData> mData;
    void AddDataWithAnalysisCheck(QList<xyzCircuitData>* dataList, xyzCircuitData newData);
public:
    DataAnalyzer(DataProcessor *dataProcessor = 0);
public slots:
    void slotInfoReceived(xyzCircuitData data);
};

#endif // DATAANALYZER_H
