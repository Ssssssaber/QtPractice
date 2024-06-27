#include "server.h"

Server::Server(QWidget* pwgt) : QWidget(pwgt)
{
    setWindowTitle("UdpServer");

    m_udp = new QUdpSocket(this);

    receivedDataText = new QTextEdit("Received data");
    sentDataText = new QTextEdit("Sent Data");

    QLabel* receivedLabel = new QLabel();
    QLabel* sentLabel = new QLabel();

    QGridLayout* boxLayout = new QGridLayout;
    boxLayout->addWidget(receivedLabel, 0, 0, 2, 1);
    boxLayout->addWidget(sentLabel, 0, 2, 2, 1);
    boxLayout->addWidget(receivedDataText, 1, 0, 2, 3);
    boxLayout->addWidget(sentDataText, 1, 3, 2, 3);
    setLayout(boxLayout);

    QTimer* ptimer = new QTimer(this);
    ptimer->setInterval(100);
    ptimer->start();

    dataProcessor = new DataProcessor();
    // connect(dataProcessor, SIGNAL(signalLineProcessed(QString)), this, SLOT(slotDataAdded(QString)));
    connect(dataProcessor, &DataProcessor::signalLineReceived, this, &Server::slotStringReceived);
    connect(dataProcessor, &DataProcessor::signalLineProcessed, this, &Server::slotDataToSendAdded);
    dataProcessor->readDataFromTestFile();

    connect(ptimer, SIGNAL(timeout()), SLOT(slotSendDatagram()));
}

void Server::slotSendDatagram()
{
    QByteArray baDatagram;
    QDataStream out(&baDatagram, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_12);
    QDateTime dt = QDateTime::currentDateTime();
    if (dataToSend.isEmpty()) return;
    xyzCircuitData data = dataToSend.dequeue();
    sentDataText->append("Sent: " + dt.toString() + "\n" + data.toString());
    out << dt << data.toString();
    m_udp->writeDatagram(baDatagram, QHostAddress::LocalHost, 2424);
}

void Server::slotStringReceived(QString string)
{
    receivedDataText->append(string);
}


void Server::slotDataToSendAdded(xyzCircuitData data)
{
    dataToSend.enqueue(data);
}
