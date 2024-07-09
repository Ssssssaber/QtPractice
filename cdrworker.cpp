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
    int r = cdr->setCircuitParams();
    emit finished(r);
    return;
}

void CDRWorker::stop()
{
    if(cdr != NULL)
        cdr->stopConfigExec();
    return;
}
