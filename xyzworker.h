#ifndef XYZWORKER_H
#define XYZWORKER_H

#include "xyzcircuitdata.h"
#include <QtCore>
#include "P7_Trace.h"

class XyzWorker : public QObject
{
    Q_OBJECT
public:
    IP7_Trace *p7Trace;
    IP7_Trace::hModule moduleName;
    XyzWorker();
public:
    virtual void slotDoWork(QList<xyzCircuitData> data);
signals:
    void signalResultReady(xyzAnalysisResult result);
};

#endif // XYZWORKER_H
