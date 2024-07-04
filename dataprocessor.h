// #ifndef DATAPROCESSOR_H
// #define DATAPROCESSOR_H

#pragma once

#include <QFile>
#include <QDebug>
#include "xyzcircuitdata.h"
class DataProcessor : public QObject
{
    Q_OBJECT

private:
    const float aConstant = 9.81f;
    const float gConstant = 1.0f;
    const float mConstant = 1.0f;
    const long timeConstant = 10000000.0f;

    QFile *file;
    QTimer* readTimer;

    int lastReceivedId = 0;
    QMap<QString, int> dataMap;

    void processLine(QString line);
    xyzCircuitData stringDataToStruct(QList<QString> tokens, float transitionConst);

public:
    DataProcessor();
    void readDataFromTestFile();
    void readLine();
private slots:
    void slotOnPackageLoss(QString message);

signals:
    void signalLossDetected(QString data);
    void signalLineReceived(QString data);
    void signalLineProcessed(xyzCircuitData data);
};

