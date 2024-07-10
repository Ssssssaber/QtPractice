#ifndef DATAPROCESSOR_H
#define DATAPROCESSOR_H

#pragma once

#include <QFile>
#include <QDebug>
#include "CircuitConfiguration.h"
#include "xyzcircuitdata.h"

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
    //static QQueue<QString> dataQueue;
    static QQueue<xyzCircuitData> dataQueue;
    static xyzCircuitData currentData;
    static QQueue<QString> errorQueue;
    const float aConstant = 9.81f;
    const float gConstant = 1.0f;
    const float mConstant = 1.0f;
    const float timeConstant = 1000000000.0f;
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
    DataProcessor();
    ~DataProcessor();
    void readDataFromTestFile();
    static void receiveDataFromDataReceiver(xyzCircuitData);
    static void receiveErrorFromDataReceiver(QString error);
    void readLine();
    void setConfig();

public slots:
    void slotDataFromDataReceiver(xyzCircuitData data);
    void slotConfigCompleted(int);
    void slotConfigReceived(cConfig data);

signals:
    void signalLineReceived(QString data);
    void signalLineProcessed(xyzCircuitData data);
    void signalConfigError(QString);
    void signalSendCircuitMessage(QString);
};

#endif // DATAPROCESSOR_H
