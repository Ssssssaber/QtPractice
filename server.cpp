#include "server.h"
#include "CircuitConfiguration.h"
#include "GTypes.h"
#include "P7_Client.h"
#include "perfomancechecker.h"
#include <QTcpSocket>
#include <QtCore>

Server::Server(int tcpPort, int udpPort, QHostAddress clientAddress, QWidget* pwgt) : QWidget(pwgt), nextBlockSize(0)
{
    IP7_Client *p7Client = P7_Get_Shared("MyChannel");
    if (p7Client)
    {
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
    this->clientAddress = clientAddress;

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
    QTimer *receivedTimerClear = new QTimer();
    receivedTimerClear->setInterval(5000);
    connect(receivedTimerClear, &QTimer::timeout, receivedDataText, &QTextEdit::clear);
    receivedTimerClear->start();

    sentDataText = new QTextEdit("Sent Data");
    sentDataText->setReadOnly(true);
    QTimer *sentTimerClear = new QTimer();
    sentTimerClear->setInterval(5000);
    connect(sentTimerClear, &QTimer::timeout, sentDataText, &QTextEdit::clear);
    sentTimerClear->start();

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

    dataProcessor = new DataProcessor(this);
    connect(dataProcessor, &DataProcessor::signalLineReceived, this, &Server::slotStringReceived);
    connect(dataProcessor, &DataProcessor::signalLineProcessed, this, &Server::slotDataToSendAdded);
    connect(this, &Server::signalConfigReceived, dataProcessor, &DataProcessor::slotConfigReceived);
    connect(dataProcessor, &DataProcessor::signalSendCircuitMessage, this, &Server::slotSendCircuitMessageToClient);

    dataAnalyzer = new DataAnalyzer(dataProcessor);
    connect(this, &Server::signalWindowSizeChanged, dataAnalyzer, &DataAnalyzer::slotWindowSizeChanged);
    connect(this, &Server::signalTimeToClearChanged, dataAnalyzer, &DataAnalyzer::slotTimeToCleanChanged);
    connect(this, &Server::signalAnalysisToggle, dataAnalyzer, &DataAnalyzer::slotAnalysisToggled);
    connect(dataAnalyzer, &DataAnalyzer::signalAnalysisReady, this, &Server::slotAnalysisToSendAdded);
    connect(dataAnalyzer, &DataAnalyzer::signalUpdatedDeltaTime, this, &Server::slotSendDeltaTime);

    // DISABLE WHEN NOT DEBUGGING
    // dataProcessor->readDataFromTestFile();

    udpDataSocket = new QUdpSocket(this);
    QTimer* pTimer = new QTimer(this);
    pTimer->setInterval(0);
    connect(pTimer, SIGNAL(timeout()), SLOT(slotSendDatagram()));
    pTimer->start();

    p7Trace->P7_TRACE(moduleName, TM("Server started"));

    PerfomanceChecker *pc = new PerfomanceChecker();
    pc->Start();
}

void Server::slotSendDatagram()
{
    if (!clientConnected) return;

    if (!cDataToSendQueue.isEmpty())
    {
        sendData();
    }
    if (!cAnalysisToSendQueue.isEmpty())
    {
        sendAnalysis();
    }
}



void Server::sendData()
{
    QByteArray baDatagram;
    QDataStream out(&baDatagram, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_12);
    QDateTime dt = QDateTime::currentDateTime();

    xyzCircuitData data = cDataToSendQueue.dequeue();
    sentDataText->append("Sent: " + dt.toString() + "\n" + data.toString());
    out << dt << data.toString();
    udpDataSocket->writeDatagram(baDatagram, clientAddress, udpPort);

    p7Trace->P7_TRACE(moduleName, TM("Data sent: %s"), data.toString().toStdString().data());
}

void Server::sendAnalysis()
{
    QByteArray baDatagram;
    QDataStream out(&baDatagram, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_12);
    QDateTime dt = QDateTime::currentDateTime();

    xyzAnalysisResult analysis = cAnalysisToSendQueue.dequeue();
    sentDataText->append("Sent: " + dt.toString() + "\n" + analysis.toString());
    out << dt << analysis.toString();
    udpDataSocket->writeDatagram(baDatagram, clientAddress, udpPort);

    p7Trace->P7_TRACE(moduleName, TM("Analysis sent: %s"), analysis.toString().toStdString().data());
}

void Server::slotSendCircuitMessageToClient(QString string)
{
    if (string != "Received configuration is valid") string = "config failed : " + string;
    else string = "config success : " + string;
    sendToClient(clientSocket, string);
}

void Server::slotSendDeltaTime(xyzAnalysisResult analysis)
{

    sendToClient(clientSocket, QString("A, G, M delta times: %1 ms, %2 ms, %3 ms").
                            arg(analysis.x).arg(analysis.y).arg(analysis.z));
}

void Server::slotClrearSentData(int toSize)
{
    sentDataText->clear();
    // QString data = sentDataText->toPlainText();
    // QStringList strList = data.split(QRegExp("[\n]"), QString::SkipEmptyParts);
}

void Server::slotStringReceived(QString stringData)
{
    receivedDataText->append(stringData);
    p7Trace->P7_TRACE(moduleName, TM("String received : %s"), stringData.toStdString().data());
}

void Server::slotNewConnection()
{
    QTcpSocket* clientSocket = tcpServer->nextPendingConnection();
    this->clientSocket = clientSocket;
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
            serverResponse = "received \"" + messageType + ": " + message + "\"";
            sendToClient(clientSocket, serverResponse);
        }
        else
        {
            serverResponse = "cannot process that message - \"" + messageType + ": " + message + "\"";
            sendToClient(clientSocket, serverResponse);
        }
        // p7Trace->P7_INFO(0, TM("%s"), serverResponse.toStdString().data());

        nextBlockSize = 0;


    }
}

void Server::slotDataToSendAdded(xyzCircuitData data)
{
    cDataToSendQueue.enqueue(data);
}

void Server::slotAnalysisToSendAdded(xyzAnalysisResult analysis)
{
    cAnalysisToSendQueue.enqueue(analysis);

}

void Server::sendToClient(QTcpSocket *socket, const QString& str)
{
    if (!clientConnected) return;
    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_12);
    out << quint16(0) << QTime::currentTime() << str;

    out.device() -> seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    socket->write(arrBlock);

    p7Trace->P7_INFO(moduleName, TM("Server: %s"), str.toStdString().data());
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
    else if (messageType == "config")
    {
        cConfig config = parseStructFromString(message);
        configsReceived.append(config);
        result = true;
        if (result && configsReceived.length() == 3)
        {
            emit signalConfigReceived(configsReceived);
            configsReceived.clear();
        }
    }
    return result;
}


