#include "windowworker.h"

WindowWorker::WindowWorker()
{

}

xyzCircuitData WindowWorker::doWork(QList<xyzCircuitData> dataList)
{
    // if (dataList.length() == 0)
    // {
    //     p7Trace->P7_ERROR(moduleName, TM("empty list on window analysis"));
    // }
    xyzCircuitData result = {
        .group = dataList[0].group,
        .id = dataList[0].id,
        .x = 0,
        .y = 0,
        .z = 0,
        .timestamp = dataList.back().timestamp,};
    int length = dataList.length();

    foreach (xyzCircuitData data, dataList.toList())
    {
        result.x += data.x;
        result.y += data.y;
        result.z += data.z;
    }

    result.x /= length;
    result.y /= length;
    result.z /= length;

    return result;
}
