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
// struct libnii_params CircuitDataReceiver::params = {
//     .accel_freq = 0,
//     .accel_range = 2,
//     .accel_avr = 8,
//     .gyro_freq = 0,
//     .gyro_range = 3,
//     .gyro_avr = 8,
//     .magnet_duty = 0,
//     .magnet_avr = 8,
//     .press_freq = 0,
//     .press_filter = 0,
//     .nv08c_freq = 0,
//     .display_refresh = 2
// };


class CDRWorker;
class DataProcessor : public QObject
{
    Q_OBJECT

private:
    IP7_Trace *p7Trace;
    IP7_Trace::hModule moduleName;

    // struct libnii_params CircuitDataReceiver::params = {
    //     .accel_freq = 0,
    //     .accel_range = 2,
    //     .accel_avr = 8,
    //     .gyro_freq = 0,
    //     .gyro_range = 3,
    //     .gyro_avr = 8,
    //     .magnet_duty = 0,
    //     .magnet_avr = 8,
    //     .press_freq = 0,
    //     .press_filter = 0,
    //     .nv08c_freq = 0,
    //     .display_refresh = 2
    // };
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
    fullConfig setConfigParamsFromList(QList<cConfig> configs);

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
    void slotConfigReceived(QList<cConfig> configsReceived);

signals:
    void signalLineReceived(QString data);
    void signalLineProcessed(xyzCircuitData data);
    void signalConfigError(QString);
    void signalSendCircuitMessage(QString);
};

#endif // DATAPROCESSOR_H
