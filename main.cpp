#include <QApplication>
#include "client.h"
#include "server.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Server server = Server();
    server.show();

    Client client = Client();
    client.show();



    return a.exec();
}
