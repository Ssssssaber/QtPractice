#ifndef WINDOWWORKER_H
#define WINDOWWORKER_H

#include "xyzworker.h"

class WindowWorker : public XyzWorker
{
public:
    WindowWorker();
public slots:
    void slotdoWork(QList<xyzCircuitData> dataList) override;
};

#endif // WINDOWWORKER_H
