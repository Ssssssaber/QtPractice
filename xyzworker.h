#ifndef XYZWORKER_H
#define XYZWORKER_H

#include "xyzcircuitdata.h"
#include <QtCore>

class XyzWorker : public QObject
{
    Q_OBJECT
public:
    XyzWorker();
public slots:
    virtual void slotdoWork(QList<xyzCircuitData> data);
signals:
    void signalResultReady(xyz result);
};

#endif // XYZWORKER_H
