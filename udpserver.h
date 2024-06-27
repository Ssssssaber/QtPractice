// #ifndef UDPSERVER_H
// #define UDPSERVER_H

#pragma once


#include <QtWidgets>
#include <QUdpSocket>
#include "dataprocessor.h"

class UdpServer : public QTextEdit
{
    Q_OBJECT
private:
    QQueue<QString> dataToSend;
    DataProcessor* dataProcessor;
    QUdpSocket* m_udp;
public:
    UdpServer(QWidget* pwgt = 0);

public slots:
    void kekeSlot();
    void slotDataAdded(QString dataString);
    void slotSendDatagram();

};

// #endif // UDPSERVER_H
