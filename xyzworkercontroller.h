#ifndef XYZWORKERCONTROLLER_H
#define XYZWORKERCONTROLLER_H

#include "xyzworker.h"

class XyzWorkerController : public QObject
{
    Q_OBJECT
    // XyzWorker* worker;
    XyzWorker* aWorker;
    XyzWorker* gWorker;
    XyzWorker* mWorker;

    // QThread workerThread;
    QThread aWorkerThread;
    QThread gWorkerThread;
    QThread mWorkerThread;
public:
    XyzWorkerController(WorkerTypes workerType);
    ~XyzWorkerController();
    Q_INVOKABLE void startOperating(QList<xyzCircuitData> dataList);
public slots:
    void slotHandleResults(xyzAnalysisResult result);
signals:
    void signalResultReady(xyzAnalysisResult analysis);
};

#endif // XYZWORKERCONTROLLER_H
