#ifndef XYZCIRCUITDATA_H
#define XYZCIRCUITDATA_H

#include <QtCore>

struct xyzCircuitData {
    int id;
    int x;
    int y;
    int z;
    long timestamp;

public:
    QString toString()
    {
        return QString("%1: (%2, %3, %4) %5").arg(id).arg(x).arg(y).arg(z).arg(timestamp);
        // return QString(QString::number(id) + ": " + " (" + QString::number(x) + ", " + QString::number(y) + ", " + QString::number(z) + ") " + QString::number(timestamp));
        // return std::to_string(id) + ": " + " (" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ") " + std::to_string(timestamp);
    }
};

#endif // XYZCIRCUITDATA_H
