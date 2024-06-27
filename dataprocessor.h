#ifndef DATAPROCESSOR_H
#define DATAPROCESSOR_H

#include <QFile>
#include <QDebug>

class DataProcessor
{
private:
    QMap<QString, int> dataMap;
    QList<QList<QByteArray>> aData;
    QList<QList<QByteArray>> gData;
    QList<QList<QByteArray>> mData;

    void processData();
public:
    DataProcessor();
    void readDataFromTestFile();

public slots:
    QString slotLineProcessed(QByteArray line);
};

#endif // DATAPROCESSOR_H
