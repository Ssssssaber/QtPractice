#ifndef XYZWORKER_H
#define XYZWORKER_H

#include "xyzcircuitdata.h"
#include <QtCore>

class XyzWorker : public QObject
{
    Q_OBJECT
public:
    XyzWorker();
public:
    virtual void slotDoWork(QList<xyzCircuitData> data);
signals:
    void signalResultReady(xyzAnalysisResult result);
};

#endif // XYZWORKER_H
