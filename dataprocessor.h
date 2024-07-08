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
    static QQueue<QString> dataQueue;
    const float aConstant = 9.81f;
    const float gConstant = 1.0f;
    const float mConstant = 1.0f;
    const float timeConstant = 1000000000;
    QTimer *readTimer;

    QFile *file;
    QTimer* fileTimer;

    CDRWorker *cdrworker;
    QThread *thread;

    int lastReceivedId = 0;
    QMap<QString, int> dataMap;

    void processLine(QString line);
    xyzCircuitData stringDataToStruct(QList<QString> tokens, float transitionConst);
    void readData();

public:
    DataProcessor();
    void readDataFromTestFile();
    static void receiveDataFromDataReceiver(QString);
    void readLine();
    void setConfig();

private slots:
    void slotOnPackageLoss(QString message);

public slots:
    void slotDataFromDataReceiver(QString data);
    void slotConfigCompleted(int);
    void slotConfigReceived(cConfig data);

signals:
    void signalLineReceived(QString data);
    void signalLineProcessed(xyzCircuitData data);
    void signalStopConfigExec();
    void signalConfigError(QString);
};

#endif // DATAPROCESSOR_H
