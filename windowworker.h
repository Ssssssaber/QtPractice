#ifndef WINDOWWORKER_H
#define WINDOWWORKER_H

#include "xyzworker.h"

class WindowWorker : public XyzWorker
{
public:
    WindowWorker();
public:
    void slotDoWork(QList<xyzCircuitData> dataList) override;
};

#endif // WINDOWWORKER_H
