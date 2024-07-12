#ifndef XYZWORKERCONTROLLER_H
#define XYZWORKERCONTROLLER_H

#include "xyzworker.h"
#include "windowworker.h"
#include "filterworker.h"

class XyzWorkerController : public QObject
{
    Q_OBJECT

    IP7_Trace *p7Trace;
    IP7_Trace::hModule moduleName;


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
    bool isEnabled = true;

public slots:
    void slotHandleResults(xyzAnalysisResult result);
    void slotHandleDt(char device, float dt);
    void slotHandleFilterFreq(char device, float filterFreq);
signals:
    void signalResultReady(xyzAnalysisResult analysis);
};

#endif // XYZWORKERCONTROLLER_H
