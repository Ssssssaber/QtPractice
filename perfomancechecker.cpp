#include "perfomancechecker.h"
#include "qdebug.h"
#include "linux-system-usage.hpp"

using namespace get_system_usage_linux;

PerfomanceChecker::PerfomanceChecker()
{
    IP7_Client *p7Client = P7_Get_Shared("MyChannel");
    if (p7Client)
    {
        p7Telemetry = P7_Create_Telemetry(p7Client, TM("Telemetry channel 1"), NULL);
        p7Telemetry->Create(TM("System/CPU"), 0, -1, 100, 90, 1, &l_wCpuId);
        p7Telemetry->Create(TM("System/Mem(mb)"), 0, -1, 100, 90, 1, &l_wMemId);
    }
    else
    {
        qDebug() << "trace server channel not started";
    }

    ptimer = new QTimer(this);
    ptimer->setInterval(100);
    // connect(ptimer, &QTimer::timeout, &PerfomanceChecker::telemetryLoad);
    connect(ptimer, SIGNAL(timeout()), this, SLOT(telemetryLoad()));
}

void PerfomanceChecker::Start()
{
    ptimer->start();
}

void PerfomanceChecker::Stop()
{
    ptimer->stop();
}


void PerfomanceChecker::telemetryLoad()
{
    CPU_stats cpuStats1 = read_cpu_data();
    Memory_stats mem = read_memory_data();
    l_dbCPU = (100.0f * get_cpu_usage(cpuStats, cpuStats1));
    l_dbMem = 100.0f * mem.get_memory_usage();
    p7Telemetry->Add(l_wCpuId, l_dbCPU);
    p7Telemetry->Add(l_wMemId, l_dbMem);
    cpuStats = read_cpu_data();
}
