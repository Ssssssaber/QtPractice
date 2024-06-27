#include "udpserver.h"
UdpServer::UdpServer(QWidget* pwgt) : QTextEdit(pwgt)
{
    setWindowTitle("UdpServer");

    m_udp = new QUdpSocket(this);

    QTimer* ptimer = new QTimer(this);
    ptimer->setInterval(100);
    ptimer->start();

    dataProcessor = new DataProcessor();
    connect(dataProcessor, SIGNAL(signalLineProcessed(QString)), this, SLOT(slotDataAdded(QString)));
    dataProcessor->readDataFromTestFile();

    connect(ptimer, SIGNAL(timeout()), SLOT(slotSendDatagram()));
}

void UdpServer::slotSendDatagram()
{
    QByteArray baDatagram;
    QDataStream out(&baDatagram, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_12);
    QDateTime dt = QDateTime::currentDateTime();
    if (dataToSend.isEmpty()) return;
    QString data = dataToSend.dequeue();
    append("Sent: " + dt.toString() + "\n" + data);
    out << dt << data;
    m_udp->writeDatagram(baDatagram, QHostAddress::LocalHost, 2424);
}


void UdpServer::slotDataAdded(QString dataString)
{
    dataToSend.enqueue(dataString);
}

void UdpServer::kekeSlot()
{
    qDebug() << "IDK" << "\n";
}
