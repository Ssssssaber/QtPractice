#include "dataprocessor.h"
#include <QApplication>
#include <QDir>

DataProcessor::DataProcessor()
{
    dataMap["A"] = 1;
    dataMap["G"] = 2;
    dataMap["M"] = 3;
    dataMap["p1"] = 4;

    connect(this, &DataProcessor::signalLossDetected, &DataProcessor::slotOnPackageLoss);
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
    while (file.canReadLine())
    {
        if (line != "")
        {
            processLine(line);
            line = file.readLine();
        }
    }

    // qDebug() << &aData << "\n";
    // qDebug() << &gData << "\n";
    // qDebug() << &mData << "\n";
}

// void DataProcessor::changeWindowSize(int newSize)
// {
//     windowSize = newSize;
//     emit signalWindowSizeChanged(newSize);
// }


void DataProcessor::processLine(QString line)
{

    QString processedLine = line.simplified();
    emit signalLineReceived(processedLine);
    QList<QString> tokens = line.simplified().split(' ');

    QString dataSource = tokens.data()[0];
    switch (dataMap[dataSource]) {
    case 1:
        // aData.append(stringDataToStruct(tokens, aConstant));
        emit signalLineProcessed(stringDataToStruct(tokens, aConstant));
        break;
    case 2:
        // gData.append(stringDataToStruct(tokens, gConstant));
        emit signalLineProcessed(stringDataToStruct(tokens, gConstant));
        break;
    case 3:
        // mData.append(stringDataToStruct(tokens, mConstant));
        emit signalLineProcessed(stringDataToStruct(tokens, mConstant));
        break;
    case 4:
        break;
    default:
        break;
    }
}


xyzCircuitData DataProcessor::stringDataToStruct(QList<QString> tokens, float transitionConst)
{

    xyzCircuitData data;
    data.group = tokens[0];
    data.id = tokens[1].toInt();
    data.x = tokens[2].toInt() * transitionConst;
    data.y = tokens[3].toInt() * transitionConst;
    data.z = tokens[4].toInt() * transitionConst;
    data.timestamp = (tokens[5].toLong() / timeConstant);
    if (lastReceivedId + 1 == data.id)
    {
        QString message =  QString("no packages lost");
        emit signalLossDetected(message);
    }
    else
    {
        QString message =  QString("packages from %1 to %2 were lost").arg(lastReceivedId).arg(data.id);
        emit signalLossDetected(message);
    }
    lastReceivedId = data.id;

    return data;
}

void DataProcessor::slotOnPackageLoss(QString message)
{
    qDebug() << message;
}
