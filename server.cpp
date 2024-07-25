#include "server.h"
#include "CircuitConfiguration.h"
#include "GTypes.h"
#include "P7_Client.h"
#include "perfomancechecker.h"
#include <QTcpSocket>
#include <QtCore>
#include "dataprocessor.h"

Server::Server(int tcpPort, int udpPort, QHostAddress clientAddress, QWidget* pwgt) : QWidget(pwgt), nextBlockSize(0)
{
    IP7_Client *p7Client = P7_Get_Shared("MyChannel");
    if (p7Client)
    {
        p7Trace = P7_Create_Trace(p7Client, TM("ServerChannel"));
        p7Trace->Share("ServerChannel");

        p7TraceData = P7_Create_Trace(p7Client, TM("ServerDataChannel"));
        p7TraceData->Share("ServerDataChannel");

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
    serverConnected = true;


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

    circuitManger = new CircuitManager(this);
    connect(this, &Server::signalConfigReceived, circuitManger, &CircuitManager::slotConfigReceived);
    connect(this, &Server::signalAnalysisToggle, circuitManger, &CircuitManager::slotSetAnalysisActive);
    connect(this, &Server::signalWindowSizeChanged, circuitManger, &CircuitManager::slotWindowSizeChanged);
    connect(this, &Server::signalTimeToClearChanged, circuitManger, &CircuitManager::slotTimeToCleanChanged);

    connect(circuitManger, &CircuitManager::signalSendCircuitMessage, this, &Server::slotSendCircuitMessageToClient);
    connect(circuitManger, &CircuitManager::signalUpdatedDeltaTime, this, &Server::slotSendDeltaTime);

    dataProcessor = new DataProcessor(udpPort, clientAddress, circuitManger, this);
    connect(dataProcessor, &DataProcessor::signalDataProcessed, circuitManger, &CircuitManager::slotAddData);

    connect(&dataThread, &QThread::finished, dataProcessor, &QObject::deleteLater);
    connect(this, &Server::signalStartProcessingData, dataProcessor, &DataProcessor::slotStart);

    dataProcessor->moveToThread(&dataThread);

    dataThread.start();

    p7Trace->P7_TRACE(moduleName, TM("Server started"));

    PerfomanceChecker *pc = new PerfomanceChecker("Telemetry server", this);
    pc->Start();
}

Server::~Server()
{
    serverConnected = false;
    dataThread.quit();
    dataThread.wait();
    circuitManger->disconnectCircuit();
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

    emit signalStartProcessingData();
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
    else if (messageType == "toggle")
    {
        QList<QString> tokens = message.simplified().split(' ');
        result = tokens[0] == "window"; // || analysisType == "filter";
        if (result)
        {
            bool isActive = tokens[1].toInt();
            emit signalAnalysisToggle(tokens[0], isActive);
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

bool Server::isServerActive()
{
    return serverConnected;
}


