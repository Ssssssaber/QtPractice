#ifndef DATAANALYZER_H
#define DATAANALYZER_H

#include "dataprocessor.h"
#include "xyzcircuitdata.h"
#include "xyzworkercontroller.h"
#include "P7_Trace.h"
#include <QtCore>

class DataAnalyzer : public QObject
{
    Q_OBJECT
private:
    IP7_Trace *p7Trace;
    IP7_Trace::hModule moduleName;
    int windowSize = 10;
    DataProcessor *dataProcessor;
    XyzWorkerController *windowWorkerController;
    QList<xyzCircuitData> aData;
    QList<xyzCircuitData> gData;
    QList<xyzCircuitData> mData;

    int dataLifespanInSeconds = 10;
    int analysisFrequency = 1;
    int inCount = 0;

    int timeToTimeout = 5000; // miliseconds
    QTimer *cleanupTimer;
    void cleanup();
    void cleanDataListToTime(QList<xyzCircuitData> *dataToClean, int timeInSeconds);

    void addDataWithAnalysisCheck(QList<xyzCircuitData>* dataList, xyzCircuitData newData);
    QList<xyzCircuitData> createListSlice(QList<xyzCircuitData> dataList, int size);
public:
    DataAnalyzer(DataProcessor *dataProcessor = 0);
public slots:
    void slotInfoReceived(xyzCircuitData data);
    void slotWindowSizeChanged(int newSize);
    void slotTimeToCleanChanged(int newTime);
    void slotAnalysisToggled(QString analysisType);
private slots:
    void slotResultReceived(xyzAnalysisResult analysis);
signals:
    void signalAnalysisReady(xyzAnalysisResult analysis);
};

#endif // DATAANALYZER_H
