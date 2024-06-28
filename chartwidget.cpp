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
    // works
    // serX->append(2,2);
    // serX->append(20,20);

    chart->addSeries(serX);
    chart->addSeries(serY);
    chart->addSeries(serZ);

    // doesnt work
    serX->append(2,3);

    chart->createDefaultAxes(); // adding DefaultAxes lines to chart

    cView = new QChartView(chart); // turning chart to widget

    QGridLayout* boxLayout = new QGridLayout; // grid
    boxLayout->addWidget(cView);
    setLayout(boxLayout);
    // doesnt work
    serX->append(2,5);
    serX->clear();
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
