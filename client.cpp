#include "client.h"

Client::Client(QWidget* pwgt) : QWidget(pwgt)
{
    setWindowTitle("UdpClient");
    receivedData = new QTextEdit();

    chartManager = new ChartManager();

    QGridLayout* boxLayout = new QGridLayout;
    boxLayout->addWidget(receivedData);
    boxLayout->addWidget(chartManager);
    setLayout(boxLayout);

    m_udp = new QUdpSocket(this);
    m_udp->bind(2424);
    connect(this, &Client::signalClientReceivedData, chartManager, &ChartManager::slotDataReceived);
    connect(m_udp, SIGNAL(readyRead()), SLOT(slotProcessDatagrams()));
}

xyzCircuitData Client::parseReceivedData(QString stringData)
{
    QList<QString> tokens = stringData.simplified().split(' ');
    xyzCircuitData data;
    data.group = tokens[0];
    data.id = tokens[1].toInt();
    data.x = tokens[2].toInt();
    data.y = tokens[3].toInt();
    data.z = tokens[4].toInt();
    data.timestamp = tokens[5].toLong();
    return data;
}

void Client::slotProcessDatagrams()
{
    QByteArray baDatagram;
    do {
        baDatagram.resize(m_udp->pendingDatagramSize());
        m_udp->readDatagram(baDatagram.data(), baDatagram.size());
    } while (m_udp->hasPendingDatagrams());

    QDateTime dateTime;
    QString data;
    QDataStream in(&baDatagram, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_12);
    in >> dateTime >> data;
    xyzCircuitData parsedData = parseReceivedData(data);
    emit signalClientReceivedData(parsedData);
    receivedData->append("Received: " + dateTime.toString() + "\n" + data);
}

