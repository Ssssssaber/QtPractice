#ifndef CIRCUITMANAGER_H
#define CIRCUITMANAGER_H

#include "CircuitConfiguration.h"
#include "cdrworker.h"
#include "datacontainer.h"
#include <QtCore>
#include <vector>

const bool noCircuit = false;

const ulong timeConstant = 1e6;
// const bool noCircuit = true;

class CircuitManager : public QObject
{
    Q_OBJECT
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

    DataContainer *container;

    // std::vector<xyzCircuitData> aData;
    // std::vector<xyzCircuitData> gData;
    // std::vector<xyzCircuitData> mData;

    QElapsedTimer *elapsedTimer;
    QRandomGenerator prng1;

    CDRWorker *cdrworker;
    QThread *thread;

    int dataLifespanInSeconds = 10;

    int windowSize = 10;
    bool windowEnabled = false;

    static QQueue<QString> errorQueue;
public:
    CircuitManager(QObject *p = 0);
    ~CircuitManager();
    void disconnectCircuit();

    fullConfig setConfigParamsFromList(QList<cConfig> configs);
    static void receiveErrorFromDataReceiver(QString error);
    float getAverageDeltaTime(std::vector<xyzCircuitData> dataVector);
    void setConfig();

    int getWindowSize();
    bool isWindowEnabled();
    fullConfig getFullConfig();
    std::vector<xyzCircuitData> *getData(char group);


private slots:
    void slotUpdateDeltaTime();
    void slotReadError();
    void slotGenerateFakeData();

public slots:
    void slotConfigCompleted(int);
    void slotWindowSizeChanged(int newSize);
    void slotSetAnalysisActive(QString analysisType, bool active);
    void slotTimeToCleanChanged(int newTime);
    void slotConfigReceived(QList<cConfig> configsReceived);
    void slotAddData(xyzCircuitData data);

signals:
    void signalConfigError(QString);
    void signalSendCircuitMessage(QString);
    void signalUpdatedDeltaTime(xyzAnalysisResult deltaTimes);
};

#endif // CIRCUITMANAGER_H
