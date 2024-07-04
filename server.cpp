#include "server.h"
#include <QTcpSocket>



Server::Server(int tcpPort, int udpPort, QWidget* pwgt) : QWidget(pwgt), nextBlockSize(0)
{
    setWindowTitle("UdpServer");

    this->tcpPort = tcpPort;
    this->udpPort = udpPort;

    tcpServer = new QTcpServer(this);
    if (!tcpServer->listen(QHostAddress::Any, tcpPort))
    {
        QMessageBox::critical(
            0,
            "Server Error",
            "Unable to start the server:"
                + tcpServer->errorString()
            );
        tcpServer->close();
        return;
    }
    connect(tcpServer, &QTcpServer::newConnection, this, &Server::slotNewConnection);



    receivedDataText = new QTextEdit("Received data");
    receivedDataText->setReadOnly(true);

    sentDataText = new QTextEdit("Sent Data");
    sentDataText->setReadOnly(true);

    clientResponseText = new QTextEdit("Client response");
    clientResponseText->setReadOnly(true);

    QLabel *receivedLabel = new QLabel("Received Data");
    QLabel *sentLabel = new QLabel("Sent Data");
    QLabel *clientLabel = new QLabel("Client response label");

    QGridLayout* boxLayout = new QGridLayout;
    boxLayout->addWidget(receivedLabel, 0, 0, 1, 3);
    boxLayout->addWidget(sentLabel, 0, 4, 1, 3);
    boxLayout->addWidget(clientLabel, 0, 8, 1, 3);
    boxLayout->addWidget(receivedDataText, 1, 0, 2, 3);
    boxLayout->addWidget(sentDataText, 1, 4, 2, 3);
    boxLayout->addWidget(clientResponseText, 1, 7, 2, 3);
    setLayout(boxLayout);

    dataProcessor = new DataProcessor();
    connect(dataProcessor, &DataProcessor::signalLineReceived, this, &Server::slotStringReceived);
    connect(dataProcessor, &DataProcessor::signalLineProcessed, this, &Server::slotDataToSendAdded);

    dataAnalyzer = new DataAnalyzer(dataProcessor);
    connect(this, &Server::signalWindowSizeChanged, dataAnalyzer, &DataAnalyzer::slotWindowSizeChanged);
    connect(dataAnalyzer, &DataAnalyzer::signalAnalysisReady, this, &Server::slotAnalysisToSendAdded);
    // connect(this, )

    //dataReceiver = new DataReceiver();
    // connect(dataReceiver, &DataReceiver::signalDataReceived, dataProcessor, &DataProcessor::slotProcessLine);

    // DISABLE WHEN NOT DEBUGGING
    //dataProcessor->readDataFromTestFile();

    udpSocket = new QUdpSocket(this);
    QTimer* ptimer = new QTimer(this);
    ptimer->setInterval(150);
    connect(ptimer, SIGNAL(timeout()), SLOT(slotSendDatagram()));
    ptimer->start();
}

void Server::slotSendDatagram()
{
    QByteArray baDatagram;
    QDataStream out(&baDatagram, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_12);
    QDateTime dt = QDateTime::currentDateTime();
    // if (dataToSendQueue.isEmpty() || !clientConnected) return;
    // xyzCircuitData data = dataToSendQueue.dequeue();
    if (stringsToSendQueue.isEmpty() | !clientConnected) return;
    QString data = stringsToSendQueue.dequeue();
    sentDataText->append("Sent: " + dt.toString() + "\n" + data);
    out << dt << data;
    udpSocket->writeDatagram(baDatagram, QHostAddress::LocalHost, udpPort);
}

void Server::slotStringReceived(QString string)
{
    receivedDataText->append(string);
}

void Server::slotNewConnection()
{
    QTcpSocket* clientSocket = tcpServer->nextPendingConnection();
    connect(clientSocket, &QTcpSocket::disconnected, this, &QTcpSocket::deleteLater);
    connect(clientSocket, &QTcpSocket::readyRead, this, &Server::slotReadClient);
    clientConnected = true;
    sendToClient(clientSocket, "Server Response: Connected!");
}

void Server::slotReadClient()
{
    QTcpSocket *clientSocket = (QTcpSocket*)sender();
    QDataStream in(clientSocket);
    in.setVersion(QDataStream::Qt_5_12);
    for (;;)
    {
        if (!nextBlockSize)
        {
            if (clientSocket->bytesAvailable() < sizeof(quint16))
            {
                break;
            }
            in >> nextBlockSize;
        }

        if (clientSocket->bytesAvailable() < nextBlockSize)
        {
            break;
        }

        QTime time;
        QString message;
        QString messageType;
        in >> time >> messageType >> message;

        QString strMessage =
            time.toString() + " " + "Client has sent - "+ messageType + ": " + message;
        clientResponseText->append(strMessage);

        if (processClientResponse(messageType, message))
        {
            sendToClient(clientSocket, "Server Response: Received \"" + messageType + ": " + message + "\"");
        }
        else
        {
            sendToClient(clientSocket, "Server cannot process that message - \"" + messageType + ": " + message + "\"");
        }

        nextBlockSize = 0;


    }
}

void Server::slotDataToSendAdded(xyzCircuitData data)
{
    stringsToSendQueue.enqueue(data.toString());
}

void Server::slotAnalysisToSendAdded(xyzAnalysisResult analysis)
{
    stringsToSendQueue.enqueue(analysis.toString());
}

void Server::sendToClient(QTcpSocket *socket, const QString& str)
{
    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_12);
    out << quint16() << QTime::currentTime() << str;

    out.device() -> seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    socket->write(arrBlock);
}

bool Server::processClientResponse(QString messageType, QString message)
{
    bool result = false;
    if (messageType == "window")
    {
        int newSize = message.toInt();
        result = newSize >= 2 && newSize <=1024;
        if (result)
        {
            emit signalWindowSizeChanged(newSize);
        }
    }
    return result;
}


