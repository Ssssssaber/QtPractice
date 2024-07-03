#ifndef XYZWORKERCONTROLLER_H
#define XYZWORKERCONTROLLER_H

#include "xyzworker.h"

class XyzWorkerController : public QObject
{
    Q_OBJECT
    XyzWorker* worker;
    QThread workerThread;
public:
    XyzWorkerController(WorkerTypes workerType);
    ~XyzWorkerController();
    Q_INVOKABLE void startOperating(QList<xyzCircuitData> dataList);
public slots:
    void slotHandleResults(xyzAnalysisResult result);
signals:
    void signalResultReady(QString resultString);
};

#endif // XYZWORKERCONTROLLER_H
