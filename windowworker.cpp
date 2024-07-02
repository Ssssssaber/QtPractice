#include "windowworker.h"

WindowWorker::WindowWorker()
{

}

void WindowWorker::slotdoWork(QList<xyzCircuitData> dataList)
{
    xyz result = {.x = 0, .y = 0, .z = 0};
    int length = dataList.length();

    foreach (xyzCircuitData data, dataList)
    {
        result.x += data.x;
        result.y += data.y;
        result.z += data.z;
    }

    result.x /= length;
    result.y /= length;
    result.z /= length;

    qDebug() << result.toString();

    emit signalResultReady(result);
}
