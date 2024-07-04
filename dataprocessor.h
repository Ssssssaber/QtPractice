// #ifndef DATAPROCESSOR_H
// #define DATAPROCESSOR_H

#pragma once

#include <QFile>
#include <QDebug>
#include "xyzcircuitdata.h"
#include "circuitdatareceiver.h"
class DataProcessor : public QObject
{
    Q_OBJECT

private:
    static QQueue<QString> dataQueue;
    const float aConstant = 9.81f;
    const float gConstant = 1.0f;
    const float mConstant = 1.0f;
    const long timeConstant = 10000000.0f;
    QTimer *readTimer;

    int lastReceivedId = 0;



    QMap<QString, int> dataMap;
    // QList<xyzCircuitData> aData;
    // QList<xyzCircuitData> gData;
    // QList<xyzCircuitData> mData;

    void processLine(QString line);
    xyzCircuitData stringDataToStruct(QList<QString> tokens, float transitionConst);
    void readData();

    // void checkCanAnalyze(QList<xyzCircuitData> dataList);
    // QList<xyzCircuitData> createWindowDataSlice(QList<xyzCircuitData> dataList);

public:
    DataProcessor();
    void readDataFromTestFile();
    static void receiveDataFromDataReceiver(QString);
    // void changeWindowSize(int newSize);

private slots:
    void slotOnPackageLoss(QString message);

public slots:
    void slotDataFromDataReceiver(QString data);

signals:
    // void signalWindowSizeChanged(int newSize);
    // void signalWindowAnalysisStart(QList<xyzCircuitData>);
    void signalLossDetected(QString data);
    void signalLineReceived(QString data);
    void signalLineProcessed(xyzCircuitData data);
};

