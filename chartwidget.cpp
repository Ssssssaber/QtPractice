#include "chartwidget.h"

ChartWidget::ChartWidget(QWidget* pwgt) : QWidget(pwgt)
{
    chart = new QChart(); // creating main chart
    serX = new QLineSeries(); // creating lines
    serY = new QLineSeries();
    serZ = new QLineSeries();

    serX->setName("X-axis"); // setting the name
    serY->setName("Y-axis");
    serZ->setName("Z-axis");
    // serX->append(2,2);
    // serX->append(20,20);

    chart->addSeries(serX);
    chart->addSeries(serY);
    chart->addSeries(serZ);
    // serX->append(2,3);

    chart->createDefaultAxes(); // adding DefaultAxes lines to chart

    QChartView* cView = new QChartView(chart); // turning chart to widget

    QGridLayout* boxLayout = new QGridLayout; // grid
    boxLayout->addWidget(cView);
    setLayout(boxLayout);
    // serX->append(2,5);
}

void ChartWidget::slotAddData(xyzCircuitData data)
{
    serX->append(data.timestamp, data.x); //adding new point to axis-line
    serY->append(data.timestamp, data.y);
    serZ->append(data.timestamp, data.z);
}
