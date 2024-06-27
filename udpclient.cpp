#include "udpclient.h"

UdpClient::UdpClient(QWidget* pwgt) : QTextEdit(pwgt)
{
    setWindowTitle("UdpClient");

    m_udp = new QUdpSocket(this);
    m_udp->bind(2424);
    // connect(m_udp, &QUdpSocket::readyRead, &UdpClient::slotProcessDatagrams);
    // connect
    connect(m_udp, SIGNAL(readyRead()), SLOT(slotProcessDatagrams()));
}

void UdpClient::slotProcessDatagrams()
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
    append("Received: " + dateTime.toString() + "\n" + data);
}

