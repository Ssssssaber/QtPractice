#include "client.h"
#include "P7_Client.h"


Client::Client(const QString& strHost, int tcpPort, int udpPort, QWidget* pwgt) : QWidget(pwgt), nextBlockSize(0)
{
    IP7_Client *p7Client = P7_Get_Shared("MyChannel");

    if (p7Client)
    {

        p7Trace = P7_Create_Trace(p7Client, TM("ClientChannel"));
        p7Trace->Share("ClientChannel");
        p7Trace->Register_Module(TM("Client"), &moduleName);
    }
    else
    {
        qDebug() << "trace server channel not started";
    }

    setWindowTitle("UdpClient");

    this->udpPort = udpPort;
    this->tcpPort = tcpPort;

    tcpSocket = new QTcpSocket(this);
    tcpSocket->connectToHost(strHost, tcpPort);
    connect(tcpSocket, &QTcpSocket::connected, this, &Client::slotConnected);
    connect(tcpSocket, &QTcpSocket::readyRead, this, &Client::slotReadyRead);

    udpSocket = new QUdpSocket(this);
    udpSocket->bind(udpPort);
    connect(udpSocket, &QUdpSocket::readyRead, this, &Client::slotProcessDatagrams);

    serverResponseText = new QTextEdit();


    receivedCircuitData = new QTextEdit();
    receivedCircuitData->setReadOnly(true);
    chartManager = new ChartManager();
    connect(this, &Client::signalReceivedData, chartManager, &ChartManager::slotDataReceived);
    connect(this, &Client::signalReceivedAnalysis, chartManager, &ChartManager::slotAnalysisRecived);

    QGridLayout* grid = new QGridLayout;
    grid->addWidget(receivedCircuitData, 9, 0, 2, 5);
    grid->addWidget(chartManager, 0, 0, 8, 10);
    grid->addWidget(serverResponseText, 9, 5, 2, 1);

    // window part
    windowInput = new QLineEdit();

    QPushButton *windowChangeButton = new QPushButton("&Send");
    connect(windowChangeButton, &QPushButton::clicked, this, &Client::slotServerChangeWindowSize);

    QPushButton *windowToggleButton = new QPushButton("&Toggle windowing");
    connect(windowToggleButton, &QPushButton::clicked, this, &Client::slotServerToggleWindow);

    grid->addWidget(new QLabel("ChangeWindow"), 8, 7, 1, 2);
    grid->addWidget(windowInput, 9, 7, 1, 2);
    grid->addWidget(windowChangeButton, 9, 10, 1, 2);
    grid->addWidget(windowToggleButton, 10, 10, 1, 2);

    // time part
    timeToClearInput = new QLineEdit();

    QPushButton *timeToClearChangeButton = new QPushButton("&Set new time");
    connect(timeToClearChangeButton, &QPushButton::clicked, this, &Client::slotServerChangeTimeToCleanup);

    grid->addWidget(new QLabel("Change time to cleanup"), 11, 7, 1, 2);
    grid->addWidget(timeToClearInput, 12, 7, 1, 2);
    grid->addWidget(timeToClearChangeButton, 12, 10, 1, 2);

    setLayout(grid);

    p7Trace->P7_TRACE(moduleName, TM("Client started"));

    // config part
    aConfig = new CircuitConfiguratorWidget('A');
    connect(aConfig, &CircuitConfiguratorWidget::configChanged, this, &Client::slotConfigChanged);
    grid->addWidget(aConfig, 13, 0, 5, 4);

    gConfig = new CircuitConfiguratorWidget('G');
    connect(gConfig, &CircuitConfiguratorWidget::configChanged, this, &Client::slotConfigChanged);
    grid->addWidget(gConfig, 13, 5, 5, 4);

    mConfig = new CircuitConfiguratorWidget('M');
    connect(mConfig, &CircuitConfiguratorWidget::configChanged, this, &Client::slotConfigChanged);
    grid->addWidget(mConfig, 13, 10, 5, 4);
}

void Client::slotProcessDatagrams()
{
    QByteArray baDatagram;
    do {
        baDatagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(baDatagram.data(), baDatagram.size());
    } while (udpSocket->hasPendingDatagrams());

    QDateTime dateTime;
    QString stringData;
    QDataStream in(&baDatagram, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_12);
    in >> dateTime >> stringData;
    // xyzCircuitData parsedData = parseReceivedData(stringData);
    parseStringData(stringData);
    QString result = "Received: " + dateTime.toString() + " - " + stringData;
    receivedCircuitData->append(result);
}


void Client::parseStringData(QString stringData)
{
    QList<QString> tokens = stringData.simplified().split(' ');
    if (tokens[0] == "data")
    {
        xyzCircuitData data;
        data.group = tokens[1];
        data.id = tokens[2].toInt();
        data.x = tokens[3].toInt();
        data.y = tokens[4].toInt();
        data.z = tokens[5].toInt();
        data.timestamp = tokens[6].toFloat();
        p7Trace->P7_TRACE(moduleName, TM("Received data: %s"), data.toString().toStdString().data());
        emit signalReceivedData(data);
    }
    else if (tokens[0] == "analysis")
    {
        xyzAnalysisResult analysis;
        analysis.method = tokens[1];
        analysis.group = tokens[2];
        analysis.x = tokens[3].toFloat();
        analysis.y = tokens[4].toFloat();
        analysis.z = tokens[5].toFloat();
        p7Trace->P7_TRACE(moduleName, TM("Received data: %s"), analysis.toString().toStdString().data());
        emit signalReceivedAnalysis(analysis);
    }

}

void Client::sendToTcpServer(QString messageType, QString message)
{
    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_12);
    out << quint16(0) << QTime::currentTime() << messageType << message;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    tcpSocket->write(arrBlock);

    p7Trace->P7_INFO(moduleName, TM("Client sent: \" %s: %s \""), messageType.toStdString().data(), message.toStdString().data());
}


void Client::slotReadyRead()
{
    QDataStream in(tcpSocket);
    in.setVersion(QDataStream::Qt_5_12);
    for (;;)
    {
        if (!nextBlockSize)
        {
            if (tcpSocket->bytesAvailable() < sizeof(quint16))
            {
                break;
            }
            in >> nextBlockSize;
        }

        if (tcpSocket->bytesAvailable() < nextBlockSize)
        {
            break;
        }

        QTime time;
        QString str;
        in >> time >> str;

        QString response = time.toString() + " " + str;

        serverResponseText->append(response);
        p7Trace->P7_INFO(moduleName, TM("%s"), response.toStdString().data());
        nextBlockSize = 0;
    }
}

void Client::slotError(QAbstractSocket::SocketError err)
{
    QString strError ="Error: " + (err == QAbstractSocket::HostNotFoundError ? "The host was not found." :
                                   err == QAbstractSocket::RemoteHostClosedError ? "The remote host is closed." :
                                   QString(tcpSocket->errorString()));
    serverResponseText->append(strError);
    p7Trace->P7_ERROR(moduleName, TM("%s"), strError.toStdString().data());
}


void Client::slotServerChangeWindowSize()
{
    sendToTcpServer("window", windowInput->text());
}

void Client::slotServerToggleWindow()
{
    sendToTcpServer("toggle analysis", "window");
}

void Client::slotServerChangeTimeToCleanup()
{
    // change cleanup time
    int newTime = timeToClearInput->text().toInt();
    if (newTime >= 10 && newTime <= 60)
    {
        chartManager->changeDataLifeSpan(newTime);
        sendToTcpServer("change cleanup time", timeToClearInput->text());
    }

}

void Client::slotConnected()
{
    serverResponseText->append("Received the connect() signal");
}

void Client::slotConfigChanged(cConfig config)
{
    sendToTcpServer("config", config.toString());
}


