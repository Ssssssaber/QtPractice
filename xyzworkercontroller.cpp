#include "xyzworkercontroller.h"
#include "windowworker.h"



XyzWorkerController::XyzWorkerController(WorkerTypes workerType)
{
    switch (workerType)
    {
    case(WorkerTypes::WindowWorker):
        worker = new WindowWorker();

        break;

    case WorkerTypes::FilterWorker:
        qDebug() << "INVALID WORKER TYPE";
        break;
    }

    worker->moveToThread(&workerThread);
    connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
    // connect(this, &XyzWorkerController::signalOperate, worker, &::XyzWorker::slotDoWork);
    connect(worker, &XyzWorker::signalResultReady, this, &XyzWorkerController::slotHandleResults);
    workerThread.start();
}

XyzWorkerController::~XyzWorkerController()
{
    workerThread.quit();
    workerThread.wait();
}

void XyzWorkerController::startOperating(QList<xyzCircuitData> dataList)
{
    worker->slotDoWork(dataList);
}

void XyzWorkerController::slotHandleResults(xyzAnalysisResult result)
{
    qDebug() << result.toString();
    emit signalResultReady(result.toString());
}
