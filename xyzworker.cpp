#include "xyzworker.h"

XyzWorker::XyzWorker()
{
    p7Trace = P7_Get_Shared_Trace("ServerChannel");

    if (!p7Trace)
    {
        qDebug() << "worker is not tracing";
    }
    else
    {
        p7Trace->Register_Module(TM("Worker"), &moduleName);
    }
}

void XyzWorker::slotDoWork(QList<xyzCircuitData> data)
{
    p7Trace->P7_ERROR(moduleName, TM("USING AN ABSTRACT CLASS"));
}
