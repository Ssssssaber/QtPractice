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

    QMap<QString, int> dataMap;
    QList<xyzCircuitData> aData;
    QList<xyzCircuitData> gData;
    QList<xyzCircuitData> mData;
    void processLine(QString line);
    xyzCircuitData stringDataToStruct(QList<QString> tokens, float transitionConst);
public:
    DataProcessor();
    void readDataFromTestFile();

signals:
    void signalLineReceived(QString data);
    void signalLineProcessed(xyzCircuitData data);
};

