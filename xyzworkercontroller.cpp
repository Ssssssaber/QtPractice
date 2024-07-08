#include "xyzworkercontroller.h"
#include "windowworker.h"



XyzWorkerController::XyzWorkerController(WorkerTypes workerType)
{
    p7Trace = P7_Get_Shared_Trace("ServerChannel");

    if (!p7Trace)
    {
        qDebug() << "control is not tracing";
        return;
    }
    else
    {
        p7Trace->Register_Module(TM("WControl"), &moduleName);
    }

    switch (workerType)
    {
    case(WorkerTypes::WindowWorker):
        // worker = new WindowWorker();
        aWorker = new WindowWorker();
        gWorker = new WindowWorker();
        mWorker = new WindowWorker();
        break;

    case WorkerTypes::FilterWorker:
        p7Trace->P7_ERROR(moduleName, TM("invalid worker type"));
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
        p7Trace->P7_ERROR(moduleName, TM("wrong data for worker"));
    }
}

void XyzWorkerController::slotHandleResults(xyzAnalysisResult result)
{
    p7Trace->P7_TRACE(moduleName, TM("Result ready: %s"), result.toString().toStdString().data());
    emit signalResultReady(result);
}
