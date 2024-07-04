#include "server.h"
#include <QTcpSocket>

Server::Server(int tcpPort, int udpPort, QWidget* pwgt) : QWidget(pwgt)
{
    setWindowTitle("UdpServer");

    userActionsServer = new QTcpServer(this);
    if (!userActionsServer->listen(QHostAddress::Any, 2323))
    {
        QMessageBox::critical(
            0,
            "Server Error",
            "Unable to start the server:"
                + userActionsServer->errorString()
            );
    }
    connect(userActionsServer, &QTcpServer::newConnection, this, &Server::slotClientConnected);


    circuitDataSocket = new QUdpSocket(this);

    receivedDataText = new QTextEdit("Received data");
    sentDataText = new QTextEdit("Sent Data");

    QLabel* receivedLabel = new QLabel();
    QLabel* sentLabel = new QLabel();

    QGridLayout* boxLayout = new QGridLayout;
    boxLayout->addWidget(receivedLabel, 0, 0, 2, 1);
    boxLayout->addWidget(sentLabel, 0, 2, 2, 1);
    boxLayout->addWidget(receivedDataText, 1, 0, 2, 3);
    boxLayout->addWidget(sentDataText, 1, 3, 2, 3);
    setLayout(boxLayout);

    dataProcessor = new DataProcessor();
    connect(dataProcessor, &DataProcessor::signalLineReceived, this, &Server::slotStringReceived);
    connect(dataProcessor, &DataProcessor::signalLineProcessed, this, &Server::slotDataToSendAdded);

    dataAnalyzer = new DataAnalyzer(dataProcessor);

    // DISABLE WHEN NOT DEBUGGING
    //dataProcessor->readDataFromTestFile();

    QTimer* ptimer = new QTimer(this);
    ptimer->setInterval(150);
    ptimer->start();
    connect(ptimer, SIGNAL(timeout()), SLOT(slotSendDatagram()));
}

void Server::slotSendDatagram()
{
    QByteArray baDatagram;
    QDataStream out(&baDatagram, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_12);
    QDateTime dt = QDateTime::currentDateTime();
    if (circuitDataQueue.isEmpty()) return;
    xyzCircuitData data = circuitDataQueue.dequeue();
    sentDataText->append("Sent: " + dt.toString() + "\n" + data.toString());
    out << dt << data.toString();
    circuitDataSocket->writeDatagram(baDatagram, QHostAddress::LocalHost, 2424);
}

void Server::slotStringReceived(QString string)
{
    receivedDataText->append(string);
}

void Server::slotClientConnected()
{
    QTcpSocket* clientSocket = userActionsServer->nextPendingConnection();
    connect(clientSocket, &QTcpSocket::disconnected, this, &Server::slotReadClient);
}

void Server::slotReadClient()
{

}


void Server::slotDataToSendAdded(xyzCircuitData data)
{
    circuitDataQueue.enqueue(data);
}
