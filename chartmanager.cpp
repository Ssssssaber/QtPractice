#include "chartmanager.h"

ChartManager::ChartManager(QWidget* pwgt) : QWidget(pwgt)
{
    p7Trace = P7_Get_Shared_Trace("ClientChannel");

    if (!p7Trace)
    {
        qDebug() << "chart manager is not tracing";
    }
    else
    {
        p7Trace->Register_Module(TM("CMan"), &moduleName);
    }

    chartA = new ChartWidget("A");
    chartG = new ChartWidget("G");
    chartM = new ChartWidget("M");

    cleanupTimer = new QTimer(this);
    cleanupTimer->setSingleShot(false);
    cleanupTimer->setInterval(timeToTimeout);

    QVBoxLayout* vlayout = new QVBoxLayout();
    vlayout->addWidget(chartA);
    vlayout->addWidget(chartG);
    vlayout->addWidget(chartM);
    vlayout->setSpacing(0);
    setLayout(vlayout);

    connect(cleanupTimer, &QTimer::timeout, this, &ChartManager::cleanup);
    cleanupTimer->start();
}

void ChartManager::changeDataLifeSpan(int newTime)
{
    dataLifespanInSeconds = newTime;
}

void ChartManager::slotDataReceived(xyzCircuitData data)
{
    if (data.group == 'A')
    {
        chartA->setChartData(data);
    }
    else if (data.group == 'G')
    {
        chartG->setChartData(data);
    }
    else if (data.group == 'M')
    {
        chartM->setChartData(data);
    }
    else
    {
        p7Trace->P7_ERROR(moduleName, TM("Wrong data received by chart"));
        return;
    }
    p7Trace->P7_TRACE(moduleName, TM("%c chart set data"), data.group);
}

void ChartManager::slotAnalysisRecived(xyzAnalysisResult data)
{
    if (data.group == "A")
    {
        chartA->setWindowResult(data);
    }
    else if (data.group == "G")
    {
        chartG->setWindowResult(data);
    }
    else if (data.group == "M")
    {
        chartM->setWindowResult(data);
    }
    else
    {
        p7Trace->P7_ERROR(moduleName, TM("Wrong data received by chart"));
    }
}

void ChartManager::cleanup()
{
    chartA->cleanAllSeries(dataLifespanInSeconds);
    chartG->cleanAllSeries(dataLifespanInSeconds);
    chartM->cleanAllSeries(dataLifespanInSeconds);
}
