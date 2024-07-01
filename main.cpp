#include <QApplication>
#include "client.h"
#include "server.h"

#include <QtCharts/QChartView> // to display charts
#include <QtCharts/QLegend> // to add legend to charts
#include <QtCharts/QLineSeries> // to draw line-chart
#include <QtCharts/QCategoryAxis> // to change names of axis


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Server server = Server();
    server.show();

    Client client = Client();
    client.show();



    return a.exec();
}
