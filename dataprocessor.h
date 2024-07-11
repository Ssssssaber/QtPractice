#ifndef DATAPROCESSOR_H
#define DATAPROCESSOR_H

#pragma once

#include <QFile>
#include <QDebug>
#include "CircuitConfiguration.h"
#include "xyzcircuitdata.h"

#include "CircuitConfiguration.h"
#include "circuitdatareceiver.h"
#include "cdrworker.h"
#include "P7_Trace.h"

class CDRWorker;
class DataProcessor : public QObject
{
    Q_OBJECT

private:
    IP7_Trace *p7Trace;
    IP7_Trace::hModule moduleName;


    cConfig currentAConfig = {
        .type = "A",
        .freq = 0,
        .avg = 8,
        .range = 2
    };
    cConfig currentGConfig = {
        .type = "G",
        .freq = 0,
        .avg = 8,
        .range = 3
    };
    cConfig currentMConfig = {
        .type = "M",
        .freq = 0,
        .avg = 8
    };

    cConfig newAConfig = currentAConfig;
    cConfig newGConfig = currentGConfig;
    cConfig newMConfig = currentMConfig;

    static QQueue<xyzCircuitData> dataQueue;
    static xyzCircuitData currentData;
    static QQueue<QString> errorQueue;
    const float mConstant = .25f * .0001f; // mT
    QMap<int, float> aMap;
    QMap<int, float> gMap;
    const float timeConstant = 1000000000;
    QTimer *dataTimer;
    QTimer *errorTimer;

    QElapsedTimer receiveTime;
    qint64 lastReceivedTime;


    QFile *file;
    QTimer* fileTimer;

    CDRWorker *cdrworker;
    QThread *thread;

    int lastReceivedId = 0;
    QMap<char, int> dataMap;

    void processLine(QString line);
    xyzCircuitData transformXyzData(xyzCircuitData data, float transitionConst);
    void processReceivedData(xyzCircuitData data);
    xyzCircuitData stringDataToStruct(QList<QString> tokens, float transitionConst);
    void readData();
    void readError();

public:
    explicit DataProcessor(QObject *parent = nullptr);
    ~DataProcessor();
    void readDataFromTestFile();
    static void receiveDataFromDataReceiver(xyzCircuitData);
    static void receiveErrorFromDataReceiver(QString error);
    void readLine();
    void setConfig();

public slots:
    void slotConfigCompleted(int);
    void slotConfigReceived(cConfig data);

signals:
    void signalLineReceived(QString data);
    void signalLineProcessed(xyzCircuitData data);
    void signalConfigError(QString);
    void signalSendCircuitMessage(QString);
};

#endif // DATAPROCESSOR_H
