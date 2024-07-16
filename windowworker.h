#ifndef WINDOWWORKER_H
#define WINDOWWORKER_H

#include <QtCore>
#include <xyzcircuitdata.h>

class WindowWorker : QObject
{
    Q_OBJECT
public:
    bool isEnabled = false;
    WindowWorker();
public:
    xyzCircuitData doWork(std::vector<xyzCircuitData> dataList);
};

#endif // WINDOWWORKER_H
