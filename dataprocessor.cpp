#include "dataprocessor.h"
#include <QApplication>
#include <QDir>
#include <QtEndian>




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

    QString line = file.readLine().simplified();
    while (file.canReadLine()) {
        // QByteArray line = file.readLine().simplified();
        // QList<QByteArray> tokens = line.split(' ');
        if (line != "")
        {
            processLine(line);
            line = file.readLine();
        }
    }

    qDebug() << &aData << "\n";
    qDebug() << &gData << "\n";
    qDebug() << &mData << "\n";
}

void DataProcessor::processLine(QString line)
{

    QString processedLine = line.simplified();
    emit signalLineReceived(processedLine);
    QList<QString> tokens = line.simplified().split(' ');

    QString dataSource = tokens.data()[0];
    switch (dataMap[dataSource]) {
    case 1:
        aData.append(stringDataToStruct(tokens, aConstant));
        break;
    case 2:
        gData.append(stringDataToStruct(tokens, gConstant));
        break;
    case 3:
        mData.append(stringDataToStruct(tokens, mConstant));
        break;
    case 4:
        break;
    default:
        break;
    }
    // QObject::dumpObjectInfo();

    // server->slotDataAdded(processedLine);
}


xyzCircuitData DataProcessor::stringDataToStruct(QList<QString> tokens, float transitionConst)
{
    xyzCircuitData data;
    data.id = tokens[1].toInt();
    data.x = tokens[2].toInt() * transitionConst;
    data.y = tokens[3].toInt() * transitionConst;
    data.z = tokens[4].toInt() * transitionConst;
    data.timestamp = tokens[5].toLong();
    emit signalLineProcessed(data);
    return data;
}
