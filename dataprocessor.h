// #ifndef DATAPROCESSOR_H
// #define DATAPROCESSOR_H

#pragma once

#include <QFile>
#include <QDebug>


class DataProcessor : public QObject
{
    Q_OBJECT
private:
    // UdpServer* server;
    QMap<QString, int> dataMap;
    QList<QList<QByteArray>> aData;
    QList<QList<QByteArray>> gData;
    QList<QList<QByteArray>> mData;
    void processLine(QByteArray line);
public:
    DataProcessor();
    void readDataFromTestFile();


signals:
    void kekeSignal();
    void signalLineProcessed(QString line);
};

// #endif // DATAPROCESSOR_H
