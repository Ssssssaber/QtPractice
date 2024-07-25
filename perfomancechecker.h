#ifndef PERFOMANCECHECKER_H
#define PERFOMANCECHECKER_H


#include "linux-system-usage.hpp"
// #include "qtimer.h"
#include "P7_Telemetry.h"
#include "qobject.h"
#include "qtimer.h"

class PerfomanceChecker : public QObject
{
    Q_OBJECT
    QTimer* ptimer;

    IP7_Telemetry *p7Telemetry;
    tUINT16 l_wCpuId = 0;
    tUINT16 l_wMemId = 0;
    tDOUBLE l_dbCPU = 0;
    tDOUBLE l_dbMem = 0;
    get_system_usage_linux::CPU_stats cpuStats;

public:
    PerfomanceChecker(QString channelName, QObject *parent = 0);
    void Start();
    void Stop();
private slots:
    void telemetryLoad();
};

#endif // PERFOMANCECHECKER_H
