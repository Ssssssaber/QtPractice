#include "chartmanager.h"
#include "qgridlayout.h"

ChartManager::ChartManager(QWidget* pwgt) : QWidget(pwgt)
{
    chartA = new ChartWidget();
    chartG = new ChartWidget();
    chartM = new ChartWidget();


    QGridLayout* boxLayout = new QGridLayout;
    boxLayout->addWidget(chartA, 0, 0, 5, 5);
    boxLayout->addWidget(chartG, 0, 5, 5, 5);
    boxLayout->addWidget(chartM, 0, 11, 5, 5);
    setLayout(boxLayout);


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
