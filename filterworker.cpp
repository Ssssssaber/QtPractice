#include "filterworker.h"

FilterWorker::FilterWorker() {}

void FilterWorker::slotDoWork(QList<xyzCircuitData> dataList)
{
    if (dataList.length() == 0)
    {
        p7Trace->P7_ERROR(moduleName, TM("empty list on filter analysis"));
        return;
    }
    xyzAnalysisResult result = {
        .method="filter",
        .group = QString(dataList[0].group),
        .x = 0,
        .y = 0,
        .z = 0
    };

    foreach (xyzCircuitData data, dataList.toList())
    {
        result.x = result.x * (1. - k) + data.x * k;
        result.y = result.y * (1. - k) + data.y * k;
        result.z = result.z * (1. - k) + data.z * k;
        emit signalResultReady(result);
    }
}
