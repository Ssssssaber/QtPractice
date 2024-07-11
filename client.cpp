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

    udpDataSocket = new QUdpSocket(this);
    udpDataSocket->bind(udpPort);
    connect(udpDataSocket, &QUdpSocket::readyRead, this, &Client::slotProcessDatagram);




    // QGridLayout* grid = new QGridLayout;
    // grid->addWidget(receivedCircuitData, 9, 0, 2, 5);
    // grid->addWidget(chartManager, 0, 0, 8, 10);
    // grid->addWidget(serverResponseText, 9, 5, 4, 5);

    // main layout
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    setLayout(mainLayout);

    // data part
    QWidget *dataWidget = new QWidget(this);
    QVBoxLayout *dataVBox = new QVBoxLayout;
    dataWidget->setLayout(dataVBox);

    // chart part
    chartManager = new ChartManager(this);
    connect(this, &Client::signalReceivedData, chartManager, &ChartManager::slotDataReceived);
    connect(this, &Client::signalReceivedAnalysis, chartManager, &ChartManager::slotAnalysisRecived);
    dataVBox->addWidget(chartManager);

    // logs part
    QWidget *logsWidget = new QWidget(this);
    QHBoxLayout *logsHBox = new QHBoxLayout;

    serverResponseText = new QTextEdit();
    serverResponseText->setReadOnly(true);

    receivedCircuitData = new QTextEdit();
    receivedCircuitData->setReadOnly(true);

    logsHBox->addWidget(receivedCircuitData);
    logsHBox->addWidget(serverResponseText);
    logsWidget->setLayout(logsHBox);

    logsWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    dataVBox->addWidget(logsWidget);


    // window part
    QWidget *windowWidget = new QWidget();
    QGridLayout *windowGrid = new QGridLayout;

    QPushButton *windowChangeButton = new QPushButton("&Send");
    connect(windowChangeButton, &QPushButton::clicked, this, &Client::slotServerChangeWindowSize);

    QPushButton *windowToggleButton = new QPushButton("&Toggle windowing");
    connect(windowToggleButton, &QPushButton::clicked, this, &Client::slotServerToggleWindow);

    windowInput = new QLineEdit();

    windowGrid->addWidget(new QLabel("ChangeWindow"), 0, 0, 1, 2);
    windowGrid->addWidget(windowInput, 1, 0, 1, 2);
    windowGrid->addWidget(windowChangeButton, 1, 3, 1, 2);
    windowGrid->addWidget(windowToggleButton, 2, 3, 1, 2);

    windowWidget->setLayout(windowGrid);

    // time part
    QWidget *timeWidget = new QWidget();
    QGridLayout *timeGrid = new QGridLayout;

    timeToClearInput = new QLineEdit();

    QPushButton *timeToClearChangeButton = new QPushButton("&Set new time");
    connect(timeToClearChangeButton, &QPushButton::clicked, this, &Client::slotServerChangeTimeToCleanup);

    timeGrid->addWidget(new QLabel("Change time to cleanup"), 0, 0, 1, 2);
    timeGrid->addWidget(timeToClearInput, 1, 0, 1, 2);
    timeGrid->addWidget(timeToClearChangeButton, 1, 3, 1, 2);

    timeWidget->setLayout(timeGrid);

    p7Trace->P7_TRACE(moduleName, TM("Client started"));

    // config part
    QWidget *configs = new QWidget(this);
    QVBoxLayout *configVBox = new QVBoxLayout;

    aConfig = new CircuitConfiguratorWidget('A');
    connect(aConfig, &CircuitConfiguratorWidget::configChanged, this, &Client::slotConfigChanged);


    gConfig = new CircuitConfiguratorWidget('G');
    connect(gConfig, &CircuitConfiguratorWidget::configChanged, this, &Client::slotConfigChanged);


    mConfig = new CircuitConfiguratorWidget('M');
    connect(mConfig, &CircuitConfiguratorWidget::configChanged, this, &Client::slotConfigChanged);


    QPushButton *setButton = new QPushButton("Change config");
    connect(setButton, &QPushButton::clicked, aConfig, &CircuitConfiguratorWidget::slotPrepeareConfig);
    connect(setButton, &QPushButton::clicked, gConfig, &CircuitConfiguratorWidget::slotPrepeareConfig);
    connect(setButton, &QPushButton::clicked, mConfig, &CircuitConfiguratorWidget::slotPrepeareConfig);

    QSpacerItem *configSpacer = new QSpacerItem(QSizePolicy::Expanding, QSizePolicy::Expanding);
    configVBox->addWidget(aConfig);
    configVBox->addWidget(gConfig);
    configVBox->addWidget(mConfig);
    configVBox->addWidget(setButton);
    configVBox->addWidget(windowWidget);
    configVBox->addWidget(timeWidget);
    configVBox->addItem(configSpacer);

    configs->setLayout(configVBox);
    configs->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    mainLayout->addWidget(dataWidget);
    mainLayout->addWidget(configs);

    QTimer *statTimer = new QTimer();
    statTimer->setInterval(3000);
    statTimer->setSingleShot(false);

    connect(statTimer, &QTimer::timeout, this, &Client::slotUpdateThreadInfo);
    statTimer->start();
}

void Client::slotProcessDatagram()
{
    QByteArray baDatagram;
    do {
        baDatagram.resize(udpDataSocket->pendingDatagramSize());
        udpDataSocket->readDatagram(baDatagram.data(), baDatagram.size());
        QDateTime dateTime;
        QString stringData;
        QDataStream in(&baDatagram, QIODevice::ReadOnly);
        in.setVersion(QDataStream::Qt_5_12);
        in >> dateTime >> stringData;
        // xyzCircuitData parsedData = parseReceivedData(stringData);
        parseStringData(stringData);
        QString result = "Received: " + dateTime.toString() + " - " + stringData;
        receivedCircuitData->append(result);
    } while (udpDataSocket->hasPendingDatagrams());
}

void Client::parseStringData(QString stringData)
{
    QList<QString> tokens = stringData.simplified().split(' ');
    if (tokens[0] == "data")
    {
        xyzCircuitData data;
        data.group = tokens[1].toStdString()[0];
        data.id = tokens[2].toInt();
        data.x = tokens[3].toFloat();
        data.y = tokens[4].toFloat();
        data.z = tokens[5].toFloat();
        data.timestamp = tokens[6].toFloat();
        p7Trace->P7_TRACE(moduleName, TM("Received data: %s"), data.toString().toStdString().data());
        // currentThread += 1;
        currentThread += sizeof(data);
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
        currentThread += sizeof(analysis);
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

void Client::processServerResponse(QString message)
{
    QList<QString> mTokens = message.simplified().split(' ');
    if (mTokens[2] == "failed")
    {
        serverResponseText->append("Client: resetting old config");
        aConfig->resetConfig();
        gConfig->resetConfig();
        mConfig->resetConfig();
    }
    else if (mTokens[2] == "success")
    {
        aConfig->keepConfig();
        gConfig->keepConfig();
        mConfig->keepConfig();
    }
    // if (mTokens[1] == "config")
    // {
    //     configStrings.append(message);
    //     if (configStrings.length() == 3)
    //     {
    //         bool error = false;
    //         foreach (QString str, configStrings)
    //         {
    //             QList<QString> tokens= str.simplified().split(' ');
    //             if (tokens[2] == "failed")
    //             {
    //                 error = true;
    //                 break;
    //             }
    //         }

    //         if (error)
    //         {
    //             serverResponseText->append("Client: resetting old config");
    //             aConfig->resetConfig();
    //             gConfig->resetConfig();
    //             mConfig->resetConfig();
    //         }
    //         else
    //         {
    //             aConfig->keepConfig();
    //             gConfig->keepConfig();
    //             mConfig->keepConfig();
    //         }

    //         configStrings.clear();
    //     }
    // }

}

void Client::slotUpdateThreadInfo()
{
    float current = currentThread / 3;
    serverResponseText->append(QDateTime::currentDateTime().toString() +
                               QString(" Client: Current thread - %1 bytes/s").arg(current));
    currentThread = 0;
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

        processServerResponse(response);

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
    serverResponseText->append(QDateTime::currentDateTime().toString() +
                               "Client: Received the connect() signal");
}

void Client::slotConfigChanged(cConfig config)
{
    sendToTcpServer("config", config.toString());
}




