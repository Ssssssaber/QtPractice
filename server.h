#pragma once

#include <QtWidgets>
#include <QUdpSocket>
#include <QTcpServer>
#include "dataanalyzer.h"
#include "dataprocessor.h"

class Server : public QWidget
{
    Q_OBJECT
private:
    QTextEdit* receivedDataText;
    QTextEdit* sentDataText;
    QQueue<xyzCircuitData> circuitDataQueue;
    DataProcessor *dataProcessor;
    DataAnalyzer *dataAnalyzer;
    QUdpSocket* circuitDataSocket;
    QTcpServer* userActionsServer;
    void sendToClient(QTcpSocket* socket, const QString);
public:
    Server(int tcpPort, int udpPort, QWidget* pwgt = 0);

public slots:
    void slotStringReceived(QString string);
    void slotClientConnected();
    void slotReadClient();
    void slotDataToSendAdded(xyzCircuitData data);
    // void slotWindowSizeChanged(int newSize);
    void slotSendDatagram();
};

// #endif // UDPSERVER_H
