#ifndef FILTERWORKER_H
#define FILTERWORKER_H

#include "xyzworker.h"

class FilterWorker : public XyzWorker
{
public:
    FilterWorker();
    float k = 0.f;
    float dt = 0.f;
public:
    void slotDoWork(QList<xyzCircuitData> dataList) override;
    void setK(float filterFreq);
};

#endif // FILTERWORKER_H
