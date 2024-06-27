#ifndef UDPSERVER_H
#define UDPSERVER_H

#include "dataprocessor.h"
#include <QtWidgets>
#include <QUdpSocket>

class UdpServer : public QTextEdit
{
    Q_OBJECT
private:
    QQueue<QString> dataToSend;
    DataProcessor dataProcessor;
    QUdpSocket* m_udp;
public:
    UdpServer(QWidget* pwgt = 0);
private slots:
    void slotSendDatagram();
    void slotDataAdded(QString dataString);
};

#endif // UDPSERVER_H
