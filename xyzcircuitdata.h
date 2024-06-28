#ifndef XYZCIRCUITDATA_H
#define XYZCIRCUITDATA_H

#include <QtCore>

struct xyzCircuitData {
    QString group;
    int id;
    int x;
    int y;
    int z;
    int timestamp;

public:
    QString toString()
    {
        return QString(group + " %1 %2 %3 %4 %5").arg(id).arg(x).arg(y).arg(z).arg(timestamp);
    }
};

#endif // XYZCIRCUITDATA_H
