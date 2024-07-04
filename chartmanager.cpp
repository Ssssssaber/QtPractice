#include "chartmanager.h"

ChartManager::ChartManager(QWidget* pwgt) : QWidget(pwgt)
{
    chartA = new ChartWidget();
    chartG = new ChartWidget();
    chartM = new ChartWidget();


    QVBoxLayout* vlayout = new QVBoxLayout();
    vlayout->addWidget(chartA);
    vlayout->addWidget(chartG);
    vlayout->addWidget(chartM);
    setLayout(vlayout);


    connect(this, &ChartManager::signalAData, chartA, &ChartWidget::slotAddData);
    connect(this, &ChartManager::signalGData, chartG, &ChartWidget::slotAddData);
    connect(this, &ChartManager::signalMData, chartM, &ChartWidget::slotAddData);
}

void ChartManager::slotDataReceived(xyzCircuitData data)
{
    if (data.group == "A")
    {
        emit signalAData(data);
    }
    if (data.group == "G")
    {
        emit signalGData(data);
    }
    if (data.group == "M")
    {
        emit signalMData(data);
    }
}
