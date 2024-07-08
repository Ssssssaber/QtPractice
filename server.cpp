#include "server.h"
#include "GTypes.h"
#include "P7_Client.h"
#include <QTcpSocket>



Server::Server(int tcpPort, int udpPort, QWidget* pwgt) : QWidget(pwgt), nextBlockSize(0)
{
    IP7_Client *p7Client = P7_Get_Shared("MyChannel");
    if (p7Client)
    {
        p7Telemetry = P7_Create_Telemetry(p7Client, TM("Telemetry channel 1"), NULL);
        p7Telemetry->Create(TM("System/CPU"), 0, -1, 100, 90, 1, &l_wCpuId);
        p7Telemetry->Create(TM("System/Mem(mb)"), 0, -1, 500, 450, 1, &l_wMemId);

        p7Trace = P7_Create_Trace(p7Client, TM("ServerChannel"));
        p7Trace->Share("ServerChannel");
        p7Trace->Register_Module(TM("Server"), &moduleName);
    }
    else
    {
        qDebug() << "trace server channel not started";
    }

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
    connect(this, &Server::signalTimeToClearChanged, dataAnalyzer, &DataAnalyzer::slotTimeToCleanChanged);
    connect(this, &Server::signalAnalysisToggle, dataAnalyzer, &DataAnalyzer::slotAnalysisToggled);
    connect(dataAnalyzer, &DataAnalyzer::signalAnalysisReady, this, &Server::slotAnalysisToSendAdded);

    // DISABLE WHEN NOT DEBUGGING
    // dataProcessor->readDataFromTestFile();

    udpSocket = new QUdpSocket(this);
    QTimer* ptimer = new QTimer(this);
    ptimer->setInterval(0);
    connect(ptimer, SIGNAL(timeout()), SLOT(slotSendDatagram()));
    ptimer->start();


    p7Trace->P7_TRACE(moduleName, TM("Server started"));
}

void Server::slotSendDatagram()
{
    QByteArray baDatagram;
    QDataStream out(&baDatagram, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_12);
    QDateTime dt = QDateTime::currentDateTime();

    if (stringsToSendQueue.isEmpty() | !clientConnected) return;
    QString data = stringsToSendQueue.dequeue();
    sentDataText->append("Sent: " + dt.toString() + "\n" + data);
    out << dt << data;
    udpSocket->writeDatagram(baDatagram, QHostAddress::LocalHost, udpPort);

    p7Trace->P7_TRACE(moduleName, TM("Datagram sent: %s"), data.toStdString().data());


    // l_dbCPU = Get_CPU_Utilization();
    // l_dbMem = Get_Mem_Utilization();
    p7Telemetry->Add(l_wCpuId, l_dbCPU);
    p7Telemetry->Add(l_wMemId, l_dbMem);
}

void Server::slotStringReceived(QString stringData)
{
    receivedDataText->append(stringData);
    p7Trace->P7_TRACE(moduleName, TM("String received : %s"), stringData.toStdString().data());
}

void Server::slotNewConnection()
{
    QTcpSocket* clientSocket = tcpServer->nextPendingConnection();
    connect(clientSocket, &QTcpSocket::disconnected, this, &QTcpSocket::deleteLater);
    connect(clientSocket, &QTcpSocket::readyRead, this, &Server::slotReadClient);
    clientConnected = true;
    sendToClient(clientSocket, "Server Response: Connected!");

    p7Trace->P7_INFO(moduleName, TM("New connection with client"));
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

        QString serverResponse;
        if (processClientResponse(messageType, message))
        {
            serverResponse = "Server Response: Received \"" + messageType + ": " + message + "\"";
            sendToClient(clientSocket, serverResponse);
        }
        else
        {
            serverResponse = "Server cannot process that message - \"" + messageType + ": " + message + "\"";
            sendToClient(clientSocket, serverResponse);
        }
        // p7Trace->P7_INFO(0, TM("%s"), serverResponse.toStdString().data());

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

    p7Trace->P7_INFO(moduleName, TM("Server sent: %s"), str.toStdString().data());
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
    else if (messageType == "change cleanup time")
    {
        int newTime = message.toInt();
        result = newTime >= 10 && newTime <= 60;
        if (result)
        {
            emit signalTimeToClearChanged(newTime);
        }
    }
    else if (messageType == "toggle analysis")
    {
        QString analysisType = message;
        result = analysisType == "window"; // || analysisType == "filter";
        if (result)
        {
            emit signalAnalysisToggle(message);
        }
    }
    return result;
}


