#include <QApplication>
#include "client.h"
#include "server.h"

#include <QtCharts/QChartView> // to display charts
#include <QtCharts/QLegend> // to add legend to charts
#include <QtCharts/QLineSeries> // to draw line-chart
#include <QtCharts/QCategoryAxis> // to change names of axis

#include <QtCore/QtCore>

static void fillSeriesRand(QLineSeries *ser){ // simple void for generating points
    ser->clear();
    for (int i =0; i < 10; i++){
        ser->append(i,QRandomGenerator::global()->bounded(0, 42));
    }
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // QWidget wgt;

    // QChart *chartA = new QChart; // creating chart for accelerator with all axies

    // QLineSeries *serAX = new QLineSeries(); // line with X-axis
    // QLineSeries *serAY = new QLineSeries();
    // QLineSeries *serAZ = new QLineSeries();

    // serAX->setName("X-axis"); // setting the name
    // serAY->setName("Y-axis");
    // serAZ->setName("Z-axis");

    // //*serAX << QPointF(11, 1) << QPointF(13, 3) << QPointF(17, 6) << QPointF(18, 3) << QPointF(20, 2); // adding new points.
    // //T.B.N. you can add new points even after adding Series to chart

    // fillSeriesRand(serAY); // adding new points.
    // fillSeriesRand(serAZ);

    // chartA->addSeries(serAX); // adding line to chart series
    // chartA->addSeries(serAY);
    // chartA->addSeries(serAZ);

    // QPushButton *rndButton = new QPushButton("Randomize"); // button creating

    // // Layout setup
    // chartA->createDefaultAxes(); // setting Defualt Grid

    // QChartView *chartAView = new QChartView; // Creating ChartView out of Chart
    // chartAView->setChart(chartA);

    // QChart *chartG = new QChart; // creating chart for gyroscope with all axies
    // QChart *chartM = new QChart; // creating chart for magnetometor with all axies

    // QChartView *chartGView = new QChartView;
    // chartGView->setChart(chartG);

    // QChartView *chartMView = new QChartView;
    // chartMView->setChart(chartM);

    // QGridLayout* cgrdLayout = new QGridLayout; // creating grid layout for charts
    // cgrdLayout->setContentsMargins(5,5,5,5);
    // cgrdLayout->setSpacing(25);
    // cgrdLayout->addWidget(chartAView, 1, 0);
    // cgrdLayout->addWidget(chartGView, 0, 0);
    // cgrdLayout->addWidget(chartMView, 0, 1);
    // cgrdLayout->addWidget(rndButton, 1, 1);

    // wgt.setLayout(cgrdLayout);
    // wgt.show();

    Server server = Server();
    server.show();

    Client client = Client();
    client.show();



    return a.exec();
}
