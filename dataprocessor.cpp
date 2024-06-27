#include "dataprocessor.h"
#include <QApplication>
#include <QDir>
DataProcessor::DataProcessor()
{
    dataMap["A"] = 1;
    dataMap["G"] = 2;
    dataMap["M"] = 3;
    dataMap["p1"] = 4;
}

void DataProcessor::readDataFromTestFile()
{
    QString appPath = QDir().absolutePath();
    qInfo() << appPath;

    QDir dir(appPath);

    bool ok;
    ok = dir.cdUp();
    assert(ok);
    ok = dir.cdUp();
    assert(ok);

    qInfo() << dir.path();
    auto keke = dir.path() + "/circuit_data/all_data.txt";
    QFile file(keke);

    if (!file.open(QIODevice::ReadOnly)) return;

    QByteArray line = file.readLine().simplified();
    while (file.canReadLine()) {
        // QByteArray line = file.readLine().simplified();
        // QList<QByteArray> tokens = line.split(' ');
        if (line != "")
        {
            processLine(line);
            line = file.readLine();
        }
    }

    qDebug() << aData << "\n";
    qDebug() << gData << "\n";
    qDebug() << mData << "\n";
}

void DataProcessor::processLine(QByteArray line)
{
    QString processedLine = line.simplified();
    QList<QByteArray> tokens = line.simplified().split(' ');

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
    // QObject::dumpObjectInfo();
    emit kekeSignal();
    emit signalLineProcessed(processedLine);
    // server->slotDataAdded(processedLine);
}

