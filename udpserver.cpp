#include "udpserver.h"
UdpServer::UdpServer(QWidget* pwgt) : QTextEdit(pwgt)
{
    setWindowTitle("UdpServer");

    m_udp = new QUdpSocket(this);

    QTimer* ptimer = new QTimer(this);
    ptimer->setInterval(500);
    ptimer->start();
    // connect(dataProcessor, SIGNAL(slotLineProcessed(QByteArray)), this, SLOT(slotDataAdded(QString)));
    connect(ptimer, SIGNAL(timeout()), SLOT(slotSendDatagram()));
}

void UdpServer::slotSendDatagram()
{
    QByteArray baDatagram;
    QDataStream out(&baDatagram, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_12);
    QDateTime dt = QDateTime::currentDateTime();
    append("Sent: " + dt.toString());
    out << dt;
    m_udp->writeDatagram(baDatagram, QHostAddress::LocalHost, 2424);
}


void UdpServer::slotDataAdded(QString dataString)
{
    dataToSend.enqueue(dataString);
}
