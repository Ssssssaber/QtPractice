#include <QApplication>
#include "client.h"
#include "server.h"
#include "P7_Client.h"
// #include "P7_Trace.h"

#include <QtCharts/QChartView> // to display charts
#include <QtCharts/QLegend> // to add legend to charts
#include <QtCharts/QLineSeries> // to draw line-chart
#include <QtCharts/QCategoryAxis> // to change names of axis
#define QT_FATAL_WARNINGS

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    IP7_Client *p7Client = P7_Create_Client(TM("/P7.Sink=Baical /P7.Addr=127.0.0.1 “/P7.Name=MyChannel”"));
    p7Client->Share("MyChannel");


    Server server = Server(2323, 2424);
    server.show();

    Client client = Client(2323, 2424);
    client.show();

    return a.exec();
}
