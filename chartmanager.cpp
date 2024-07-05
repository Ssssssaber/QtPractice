#include "chartmanager.h"

ChartManager::ChartManager(QWidget* pwgt) : QWidget(pwgt)
{
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

void ChartManager::slotDataReceived(xyzCircuitData data)
{
    if (data.group == "A")
    {
        chartA->setChartData(data);
    }
    if (data.group == "G")
    {
        chartG->setChartData(data);
    }
    if (data.group == "M")
    {
        chartM->setChartData(data);
    }
}

void ChartManager::slotAnalysisRecived(xyzAnalysisResult data)
{
    if (data.group == "A")
    {
        chartA->setWindowResult(data);
    }
    if (data.group == "G")
    {
        chartG->setWindowResult(data);
    }
    if (data.group == "M")
    {
        chartM->setWindowResult(data);
    }
}

void ChartManager::cleanup()
{
    chartA->cleanAllSeries(dataLifespanInSeconds);
    chartG->cleanAllSeries(dataLifespanInSeconds);
    chartM->cleanAllSeries(dataLifespanInSeconds);
}
