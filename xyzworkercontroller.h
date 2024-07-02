#ifndef XYZWORKERCONTROLLER_H
#define XYZWORKERCONTROLLER_H

#include "xyzworker.h"

class XyzWorkerController : public QObject
{
    Q_OBJECT
    XyzWorker* worker;
    QThread workerThread;
    // QThread xThread;
    // QThread yThread;
    // QThread zThread;
public:
    XyzWorkerController(WorkerTypes workerType);
    ~XyzWorkerController();
    void startOperating(QList<xyzCircuitData> dataList);
public slots:
    void slotHandleResults(xyz result);
signals:
    void signalOperate(QList<xyzCircuitData> dataList);
};

#endif // XYZWORKERCONTROLLER_H
