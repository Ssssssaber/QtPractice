#include "windowworker.h"

WindowWorker::WindowWorker()
{
    isEnabled = false;
}

xyzCircuitData WindowWorker::doWork(std::vector<xyzCircuitData> dataList)
{
    if (dataList.empty()) return {};

    xyzCircuitData result = {
        .group = dataList[0].group,
        .id = dataList[0].id,
        .x = 0,
        .y = 0,
        .z = 0,
        .timestamp = dataList.back().timestamp,};
    int length = dataList.size();

    int i = 0;

    std::vector<xyzCircuitData>::iterator it;
    for (it = dataList.begin(); it != dataList.end(); it++, i++)
    {
        result.x += it->x;
        result.y += it->y;
        result.z += it->z;
    }

    result.x /= length;
    result.y /= length;
    result.z /= length;

    return result;
}
