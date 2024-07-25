#ifndef XYZCIRCUITDATA_H
#define XYZCIRCUITDATA_H

#include <QtCore>

struct xyzCircuitData {
    char group;
    int id;
    float x;
    float y;
    float z;
    float timestamp;

public:
    QString toQString()
    {
        return QString("data %1 %2 %3 %4 %5 %6 %7").arg(group).arg(id).arg(x).arg(y).arg(z).arg(timestamp);
    }

    QString serialize()
    {
        QByteArray byteArray;

        QDataStream stream(&byteArray, QIODevice::WriteOnly);
        stream.setVersion(QDataStream::Qt_4_5);

        stream << group << id <<
                x << y << z <<
                timestamp;
        return byteArray;
    }

    void deserialize(const QByteArray& byteArray)
    {
        QDataStream stream(byteArray);
        stream.setVersion(QDataStream::Qt_4_5);

        stream >> group >> id >>
            x >> y >> z >>
            timestamp;
    }
};

struct xyzAnalysisResult
{
    QString method;
    QString group;
    float x = 0;
    float y = 0;
    float z = 0;

    QString toString()
    {
        return QString("analysis " + method + " " + group + " %1 %2 %3").arg(x).arg(y).arg(z);
    }
};

// struct chartBoundries{
//     float xmax;
//     float xmin;
//     float ymax;
//     float ymin;
//     float zmax;
//     float zmin;
// };

enum class WorkerTypes {WindowWorker, FilterWorker};

#endif // XYZCIRCUITDATA_H
