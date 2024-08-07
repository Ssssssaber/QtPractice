#include "client.h"
#include "P7_Client.h"
#include "perfomancechecker.h"


Client::Client(int tcpPort, int udpPort, QHostAddress serverAddress, QWidget* pwgt) : QWidget(pwgt), nextBlockSize(0)
{
    IP7_Client *p7Client = P7_Get_Shared("MyChannel");

    if (p7Client)
    {

        p7Trace = P7_Create_Trace(p7Client, TM("ClientChannel"));
        p7Trace->Share("ClientChannel");
        p7TraceData = P7_Create_Trace(p7Client, TM("ClientDataChannel"));
        p7TraceData->Share("ClientDataChannel");
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
    tcpSocket->connectToHost(serverAddress, tcpPort);
    connect(tcpSocket, &QTcpSocket::connected, this, &Client::slotConnected);
    connect(tcpSocket, &QTcpSocket::readyRead, this, &Client::slotReadyRead);

    udpDataSocket = new QUdpSocket(this);
    udpDataSocket->bind(udpPort);
    connect(udpDataSocket, &QUdpSocket::readyRead, this, &Client::slotProcessDatagram);

    // main layout
    QSplitter *mainSplitter = new QSplitter(Qt::Horizontal);

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    setLayout(mainLayout);

    // data part
    QSplitter *dataSplitter = new QSplitter(Qt::Vertical);
    dataSplitter->setStretchFactor(0, 1);
    dataSplitter->setStretchFactor(1, 0);



    // chart part
    chartManager = new ChartManager(this);
    connect(this, &Client::signalReceivedData, chartManager, &ChartManager::slotDataReceived);
    connect(this, &Client::signalDataLifespanChanged, chartManager, &ChartManager::slotDataLifespanChanged);
    connect(this, &Client::signalReceivedAnalysis, chartManager, &ChartManager::slotAnalysisRecived);

    connect(&chartThread, &QThread::finished, chartManager, &QObject::deleteLater);
    chartManager->moveToThread(&chartThread);

    chartThread.start();
    // dataVBox->addWidget(chartManager);
    dataSplitter->addWidget(chartManager);

    // logs part
    QWidget *logsWidget = new QWidget(this);
    QHBoxLayout *logsHBox = new QHBoxLayout;

    serverResponseText = new QTextEdit();
    serverResponseText->setReadOnly(true);

    // logsHBox->addWidget(receivedCircuitData);
    logsHBox->addWidget(serverResponseText);
    logsWidget->setLayout(logsHBox);

    logsWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    // dataVBox->addWidget(logsWidget);
    dataSplitter->addWidget(logsWidget);


    // window part
    QWidget *windowWidget = new QWidget();
    QGridLayout *windowGrid = new QGridLayout;

    QPushButton *windowChangeButton = new QPushButton("&Send");
    connect(windowChangeButton, &QPushButton::clicked, this, &Client::slotServerChangeWindowSize);

    windowToggleButton = new QPushButton("&Enable windowing");
    connect(windowToggleButton, &QPushButton::clicked, this, &Client::slotServerToggleWindow);

    windowInput = new QLineEdit();
    windowInput->setText("10");

    windowGrid->addWidget(new QLabel("Change Window"), 0, 0, 1, 2);
    windowGrid->addWidget(windowInput, 1, 0, 1, 2);
    windowGrid->addWidget(windowChangeButton, 1, 3, 1, 2);
    windowGrid->addWidget(windowToggleButton, 2, 3, 1, 2);

    windowWidget->setLayout(windowGrid);

    // time part
    QWidget *timeWidget = new QWidget();
    QGridLayout *timeGrid = new QGridLayout;

    timeToClearInput = new QLineEdit();
    timeToClearInput->setText("10");

    QPushButton *timeToClearChangeButton = new QPushButton("&Set new time");
    connect(timeToClearChangeButton, &QPushButton::clicked, this, &Client::slotServerChangeTimeToCleanup);

    timeGrid->addWidget(new QLabel("Change time to cleanup in seconds"), 0, 0, 1, 2);
    timeGrid->addWidget(timeToClearInput, 1, 0, 1, 2);
    timeGrid->addWidget(timeToClearChangeButton, 1, 3, 1, 2);

    timeWidget->setLayout(timeGrid);

    p7Trace->P7_TRACE(moduleName, TM("Client started"));

    // config part
    QWidget *configs = new QWidget(this);
    QVBoxLayout *configVBox = new QVBoxLayout;

    aConfig = new CircuitConfiguratorWidget('A');
    connect(aConfig, &CircuitConfiguratorWidget::signalConfigChanged, this, &Client::slotConfigChanged);


    gConfig = new CircuitConfiguratorWidget('G');
    connect(gConfig, &CircuitConfiguratorWidget::signalConfigChanged, this, &Client::slotConfigChanged);


    mConfig = new CircuitConfiguratorWidget('M');
    connect(mConfig, &CircuitConfiguratorWidget::signalConfigChanged, this, &Client::slotConfigChanged);


    QPushButton *setButton = new QPushButton("Change config");
    connect(setButton, &QPushButton::clicked, aConfig, &CircuitConfiguratorWidget::slotPrepeareConfig);
    connect(setButton, &QPushButton::clicked, gConfig, &CircuitConfiguratorWidget::slotPrepeareConfig);
    connect(setButton, &QPushButton::clicked, mConfig, &CircuitConfiguratorWidget::slotPrepeareConfig);

    // QSpacerItem *configSpacer = new QSpacerItem(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
    configVBox->addStretch();
    configVBox->addWidget(aConfig);
    configVBox->addWidget(gConfig);
    configVBox->addWidget(mConfig);
    configVBox->addWidget(setButton);
    configVBox->addWidget(windowWidget);
    configVBox->addWidget(timeWidget);
    // configVBox->addItem(configSpacer);
    configVBox->addStretch();

    configs->setLayout(configVBox);
    configs->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    mainSplitter->addWidget(dataSplitter);
    mainSplitter->addWidget(configs);
    mainSplitter->setStretchFactor(0, 1);
    mainSplitter->setStretchFactor(1, 0);
    mainLayout->addWidget(mainSplitter);

    QTimer *statTimer = new QTimer();
    statTimer->setInterval(3000);
    statTimer->setSingleShot(false);

    connect(statTimer, &QTimer::timeout, this, &Client::slotUpdateThreadInfo);
    statTimer->start();

    PerfomanceChecker *pc = new PerfomanceChecker("Telemetry client", this);
    pc->Start();
}

Client::~Client()
{
    chartThread.quit();
    chartThread.wait();
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
        currentThread += sizeof(dateTime) + sizeof(stringData);
        parseStringData(stringData);
        QString result = "Received: " + dateTime.toString() + " - " + stringData;
        // receivedCircuitData->append(result);
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
        p7TraceData->P7_TRACE(moduleName, TM("Received data: %s"), data.toQString().toStdString().data());
        emit signalReceivedData(data);
        if (windowActive)
        {
            emit signalReceivedAnalysis(data);
        }
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
        p7Trace->P7_WARNING(moduleName, TM("Resetting old config"));
    }
    else if (mTokens[2] == "success")
    {
        aConfig->keepConfig();
        gConfig->keepConfig();
        mConfig->keepConfig();
        p7Trace->P7_INFO(moduleName, TM("Config set"));
    }
}

void Client::slotUpdateThreadInfo()
{
    float current = currentThread / 3;
    serverResponseText->append(QDateTime::currentDateTime().toString() +
                               QString(" Client: Current thread - %1 bytes/s").arg(current));
    p7Trace->P7_INFO(moduleName, TM("Client: Current thread - %d bytes/s"), current);
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
    windowActive = !windowActive;
    if (!windowActive)
    {
        windowToggleButton->setText("&E&nable windowing");
        //
    }
    else
    {
        windowToggleButton->setText("&D&i&sable windowing");
    }
    sendToTcpServer("toggle", "window " + QString::number(windowActive));
}

void Client::slotServerChangeTimeToCleanup()
{
    // change cleanup time
    int newTime = timeToClearInput->text().toInt();
    if (newTime >= 10 && newTime <= 60)
    {
        emit signalDataLifespanChanged(newTime);
        // chartManager->slotDataLifespanChanged(newTime);
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




