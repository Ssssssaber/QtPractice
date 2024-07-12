#ifndef FILTERWORKER_H
#define FILTERWORKER_H

#include "xyzworker.h"

class FilterWorker : public XyzWorker
{
public:
    FilterWorker();
    float k = 0.9;
public:
    void slotDoWork(QList<xyzCircuitData> dataList) override;
};

#endif // FILTERWORKER_H
