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
    QString toString()
    {
        return QString("data %1 %2 %3 %4 %5 %6").arg(group).arg(id).arg(x).arg(y).arg(z).arg(timestamp);
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

enum class WorkerTypes {WindowWorker, FilterWorker};

#endif // XYZCIRCUITDATA_H
