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
