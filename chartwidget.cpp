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

    chart->addSeries(serX);
    chart->addSeries(serY);
    chart->addSeries(serZ);

    chart->createDefaultAxes(); // adding DefaultAxes lines to chart

    cView = new QChartView(chart); // turning chart to widget

    QGridLayout* boxLayout = new QGridLayout; // grid
    boxLayout->addWidget(cView);
    setLayout(boxLayout);
}

void ChartWidget::slotAddData(xyzCircuitData data)
{
    qDebug() << data.timestamp << data.x;
    chart->removeSeries(serX);
    chart->removeSeries(serY);
    chart->removeSeries(serZ);
    serX->append(data.timestamp, data.x); //adding new point to axis-line
    serY->append(data.timestamp, data.y);
    serZ->append(data.timestamp, data.z);
    // serX->update();
    chart->addSeries(serX);
    chart->addSeries(serY);
    chart->addSeries(serZ);
    // cView->update();
}
