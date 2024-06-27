#pragma once


#include <QtWidgets>
#include <QUdpSocket>
#include "dataprocessor.h"

class Server : public QWidget
{
    Q_OBJECT
private:
    QTextEdit* receivedDataText;
    QTextEdit* sentDataText;
    QQueue<xyzCircuitData> dataToSend;
    DataProcessor* dataProcessor;
    QUdpSocket* m_udp;
public:
    Server(QWidget* pwgt = 0);

public slots:
    void slotStringReceived(QString string);
    void slotDataToSendAdded(xyzCircuitData data);
    void slotSendDatagram();

};

// #endif // UDPSERVER_H
