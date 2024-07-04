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

    Server server = Server();
    server.show();

    Client client = Client();
    client.show();



    return a.exec();
}
