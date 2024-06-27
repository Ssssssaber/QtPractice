#include <QApplication>
#include "dataprocessor.h"
#include "udpclient.h"
#include "udpserver.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    DataProcessor dataProcessor;

    dataProcessor.readDataFromTestFile();


    UdpServer server = UdpServer();
    server.show();

    UdpClient client = UdpClient();
    client.show();

    return a.exec();
}
