#include "dataprocessor.h"

DataProcessor::DataProcessor()
{
    dataMap["A"] = 1;
    dataMap["G"] = 2;
    dataMap["M"] = 3;
    dataMap["p1"] = 4;
}

void DataProcessor::readDataFromTestFile()
{
    QFile file("/home/vladimir/Code/QtPractice/circuit_data/all_data.txt");

    if (!file.open(QIODevice::ReadOnly)) return;

    QByteArray line = file.readLine().simplified();
    while (file.canReadLine()) {
        // QByteArray line = file.readLine().simplified();
        // QList<QByteArray> tokens = line.split(' ');
        if (line != "")
        {
            slotLineProcessed(line);
            line = file.readLine();
        }
    }

    qDebug() << aData << "\n";
    qDebug() << gData << "\n";
    qDebug() << mData << "\n";
}

QString DataProcessor::slotLineProcessed(QByteArray line)
{
    QString processedLine = line.simplified();
    QList<QByteArray> tokens = line.split(' ');

    QString dataSource = tokens.data()[0];
    switch (dataMap[dataSource]) {
    case 1:
        aData.append(tokens);
        break;
    case 2:
        gData.append(tokens);
        break;
    case 3:
        mData.append(tokens);
        break;
    case 4:
        break;
    default:
        break;
    }

    return processedLine;
}
