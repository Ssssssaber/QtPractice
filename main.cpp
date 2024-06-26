#include <QApplication>
#include "udpclient.h"
#include "udpserver.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    UdpServer server = UdpServer();
    server.show();

    UdpClient client = UdpClient();
    client.show();

    return a.exec();
}

