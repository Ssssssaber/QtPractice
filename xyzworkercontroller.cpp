#include "xyzworkercontroller.h"
#include "windowworker.h"



XyzWorkerController::XyzWorkerController(WorkerTypes workerType)
{
    switch (workerType)
    {
    case(WorkerTypes::WindowWorker):
        // worker = new WindowWorker();
        aWorker = new WindowWorker();
        gWorker = new WindowWorker();
        mWorker = new WindowWorker();
        break;

    case WorkerTypes::FilterWorker:
        qDebug() << "INVALID WORKER TYPE";
        break;
    }

    // worker->moveToThread(&workerThread);
    aWorker->moveToThread(&aWorkerThread);
    gWorker->moveToThread(&gWorkerThread);
    mWorker->moveToThread(&mWorkerThread);

    // connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(&aWorkerThread, &QThread::finished, aWorker, &QObject::deleteLater);
    connect(&gWorkerThread, &QThread::finished, gWorker, &QObject::deleteLater);
    connect(&mWorkerThread, &QThread::finished, mWorker, &QObject::deleteLater);

    // connect(worker, &XyzWorker::signalResultReady, this, &XyzWorkerController::slotHandleResults);
    connect(aWorker, &XyzWorker::signalResultReady, this, &XyzWorkerController::slotHandleResults);
    connect(gWorker, &XyzWorker::signalResultReady, this, &XyzWorkerController::slotHandleResults);
    connect(mWorker, &XyzWorker::signalResultReady, this, &XyzWorkerController::slotHandleResults);

    // workerThread.start();
    aWorkerThread.start();
    gWorkerThread.start();
    mWorkerThread.start();
}

XyzWorkerController::~XyzWorkerController()
{
    // workerThread.quit();
    // workerThread.wait();

    aWorkerThread.quit();
    gWorkerThread.quit();
    mWorkerThread.quit();

    aWorkerThread.wait();
    gWorkerThread.wait();
    mWorkerThread.wait();
}

void XyzWorkerController::startOperating(QList<xyzCircuitData> dataList)
{
    // worker->slotDoWork(dataList);
    xyzCircuitData data = dataList[0];
    if (data.group == "A")
    {
       aWorker->slotDoWork(dataList);
    }
    else if (data.group == "G")
    {
        gWorker->slotDoWork(dataList);
    }
    else if (data.group == "M")
    {
        mWorker->slotDoWork(dataList);
    }
    else
    {
        qDebug() << "WRONG DATA FOR WORKER";
    }
}

void XyzWorkerController::slotHandleResults(xyzAnalysisResult result)
{
    qDebug() << result.toString();
    emit signalResultReady(result);
}
