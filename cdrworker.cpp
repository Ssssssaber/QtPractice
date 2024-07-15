#include "cdrworker.h"

CDRWorker::CDRWorker()
{
    cdr = NULL;
}

CDRWorker::~CDRWorker()
{
    if(cdr == NULL)
        delete cdr;
}

void CDRWorker::process()
{
    int r = cdr->setConfigParams();
    emit finished(r);
    return;
}

void CDRWorker::disconnectCircuit()
{
    int r = cdr->setConfigParams();
    emit finished();
    if(r == LIBNII_SUCCESS)
        qDebug() << "Default configuration is set";
    else
        qDebug() << "Failed to set default configuration";
    cdr->disconnectCircuit();
}
