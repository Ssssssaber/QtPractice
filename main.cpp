#include <QApplication>
#include "client.h"
#include "server.h"

#include <QtCharts/QChartView> // to display charts
#include <QtCharts/QLegend> // to add legend to charts
#include <QtCharts/QLineSeries> // to draw line-chart
#include <QtCharts/QCategoryAxis> // to change names of axis
#define QT_FATAL_WARNINGS

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Server server = Server(2323, 2424);
    server.show();

    Client client = Client("localhost", 2323, 2424);
    client.show();



    return a.exec();
}
