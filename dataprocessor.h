#ifndef DATAPROCESSOR_H
#define DATAPROCESSOR_H

#include "windowworker.h"

#include <QFile>
#include <QDebug>
#include "CircuitConfiguration.h"
#include "xyzcircuitdata.h"

#include "CircuitConfiguration.h"
#include "P7_Trace.h"

class CDRWorker;
class DataProcessor : public QObject
{
    Q_OBJECT

private:
    IP7_Trace *p7Trace;
    IP7_Trace::hModule moduleName;

    fullConfig defaultConfig = {
        .aFreq = 0,
        .aAvg = 8,
        .aRange = 2,
        .gFreq = 0,
        .gAvg = 8,
        .gRange = 3,
        .mFreq = 0,
        .mAvg = 8
    };

    fullConfig currentConfig = {
        .aFreq = 0,
        .aAvg = 8,
        .aRange = 2,
        .gFreq = 0,
        .gAvg = 8,
        .gRange = 3,
        .mFreq = 0,
        .mAvg = 8
    };
    fullConfig newConfig;

    static QQueue<xyzCircuitData> dataQueue;
    static xyzCircuitData currentData;
    static QQueue<QString> errorQueue;

    const float mConstant = .25f * .0001f; // mT
    QMap<int, float> aMap;
    QMap<int, float> gMap;

    const float timeConstant = 1000000000;
    QTimer *dataTimer;
    QTimer *errorTimer;

    long aLost = 0;
    long gLost = 0;
    long mLost = 0;

    QElapsedTimer receiveTime;
    qint64 lastReceivedTime;


    // QFile *file;
    // QTimer* fileTimer;

    CDRWorker *cdrworker;
    QThread *thread;

    int lastReceivedId = 0;
    QMap<char, int> dataMap;

    // XyzWorkerController *windowWorkerController;
    WindowWorker *windowWorker;
    int windowSize = 10;
    QList<xyzCircuitData> aData;
    QList<xyzCircuitData> gData;
    QList<xyzCircuitData> mData;

    int aReceived;
    int gReceived;
    int mReceived;

    int dataLifespanInSeconds = 10;
    int analysisFrequency = 1;
    int inCount = 0;

    int timeToTimeout = 5000; // miliseconds
    QTimer *cleanupTimer;

    void cleanDataListToTime(QList<xyzCircuitData> *dataToClean, int timeInSeconds);
    float getAverageDeltaTime(QList<xyzCircuitData> data, int amount = -1);

    void addDataWithAnalysisCheck(QList<xyzCircuitData>* dataList, xyzCircuitData newData);
    QList<xyzCircuitData> createListSlice(QList<xyzCircuitData> dataList, int size);

    // void processReceivedData(QString line);
    xyzCircuitData transformXyzData(xyzCircuitData data, float transitionConst);
    void processReceivedData(xyzCircuitData data);
    // xyzCircuitData stringDataToStruct(QList<QString> tokens, float transitionConst);
    void readData();
    void readError();
    fullConfig setConfigParamsFromList(QList<cConfig> configs);

public:
    explicit DataProcessor(QObject *parent = nullptr);
    ~DataProcessor();
    // void readDataFromTestFile();
    static void receiveDataFromDataReceiver(xyzCircuitData);
    static void receiveErrorFromDataReceiver(QString error);
    // void readLine();
    void setConfig();

private slots:
    void slotUpdateDeltaTime();
    void slotCleanup();

public slots:
    void slotConfigCompleted(int);
    void slotConfigReceived(QList<cConfig> configsReceived);
    void slotWindowSizeChanged(int newSize);
    void slotTimeToCleanChanged(int newTime);
    void slotSetAnalysisActive(QString analysisType, bool active);

signals:    
    void signalUpdatedDeltaTime(xyzAnalysisResult deltaTimes);
    void signalLineReceived(QString data);
    void signalLineProcessed(xyzCircuitData data);
    void signalConfigError(QString);
    void signalSendCircuitMessage(QString);
};

#endif // DATAPROCESSOR_H
