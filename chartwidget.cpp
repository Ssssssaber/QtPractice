#include "chartwidget.h"
#include "QSpacerItem"

ChartWidget::ChartWidget(QWidget* pwgt) : QWidget(pwgt)
{
    hlayout = new QHBoxLayout(); // layout
    serX = new QLineSeries(); // creating lines
    serY = new QLineSeries();
    serZ = new QLineSeries();

    serX->setName("X-axis"); // setting the names
    serY->setName("Y-axis");
    serZ->setName("Z-axis");

    cViewX = new QChartView(); // creating chart-views
    cViewY = new QChartView();
    cViewZ = new QChartView();

    cViewX->chart()->addSeries(serX);
    cViewY->chart()->addSeries(serY);
    cViewZ->chart()->addSeries(serZ);

    cViewX->chart()->createDefaultAxes(); // adding DefaultAxes lines to chart-views
    cViewY->chart()->createDefaultAxes();
    cViewZ->chart()->createDefaultAxes();

    hlayout->addWidget(cViewX); // adding widget
    hlayout->addWidget(cViewY);
    hlayout->addWidget(cViewZ);
    hlayout->addItem(new QSpacerItem(0,0,QSizePolicy::Expanding, QSizePolicy::Fixed)); // adding spacer for style

    setLayout(hlayout);
}

void ChartWidget::slotAddData(xyzCircuitData data)
{
    qDebug() << data.timestamp << data.x;

    cViewX->chart()->removeSeries(serX);
    cViewY->chart()->removeSeries(serY);
    cViewZ->chart()->removeSeries(serZ);

    serX->append(data.timestamp, data.x); //adding new point to axis-line
    serY->append(data.timestamp, data.y);
    serZ->append(data.timestamp, data.z);

    // serX->update();
    cViewX->chart()->addSeries(serX);
    cViewY->chart()->addSeries(serY);
    cViewZ->chart()->addSeries(serZ);
    // cView->update();
}
