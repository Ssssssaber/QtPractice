#include "windowworker.h"

WindowWorker::WindowWorker()
{

}

void WindowWorker::slotDoWork(QList<xyzCircuitData> dataList)
{
    if (dataList.length() == 0)
    {
        qDebug() << "ERROR: empty list on window analysis";
    }
    xyzAnalysisResult result = {.method="window", .group = dataList[0].group,.x = 0, .y = 0, .z = 0};
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

    qDebug() << dataList.length();

    emit signalResultReady(result);
}
