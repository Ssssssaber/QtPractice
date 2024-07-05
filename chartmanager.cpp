#include "chartmanager.h"

ChartManager::ChartManager(QWidget* pwgt) : QWidget(pwgt)
{
    chartA = new ChartWidget("A");
    chartG = new ChartWidget("G");
    chartM = new ChartWidget("M");


    QVBoxLayout* vlayout = new QVBoxLayout();
    vlayout->addWidget(chartA);
    vlayout->addWidget(chartG);
    vlayout->addWidget(chartM);
    vlayout->setSpacing(0);
    setLayout(vlayout);
}

void ChartManager::slotDataReceived(xyzCircuitData data)
{
    if (data.group == "A")
    {
        chartA->setChartData(data);
    }
    else if (data.group == "G")
    {
        chartG->setChartData(data);
    }
    else if (data.group == "M")
    {
        chartM->setChartData(data);
    }
    else
    {
        qDebug() << "Wrong data received by chart";
    }
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
        qDebug() << "Wrong data received by chart";
    }
}

