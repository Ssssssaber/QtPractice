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
    DataProcessor *dataProcessor;
    DataAnalyzer *dataAnalyzer;

    int udpPort;
    QUdpSocket* udpSocket;
    QTextEdit* receivedDataText;
    QTextEdit* sentDataText;
    QQueue<xyzCircuitData> dataToSendQueue;

    int tcpPort;
    bool clientConnected = false;
    QTcpServer* tcpServer;
    QTextEdit *clientResponseText;
    quint16 nextBlockSize;
    void sendToClient(QTcpSocket* socket, const QString &str);
    bool processClientResponse(QString messageType, QString message);
public:
    Server(int tcpPort, int udpPort, QWidget* pwgt = 0);

public slots:
    void slotStringReceived(QString string);
    void slotNewConnection();
    void slotReadClient();
    void slotDataToSendAdded(xyzCircuitData data);
    void slotSendDatagram();
signals:
    void signalWindowSizeChanged(int newSize);
};

// #endif // UDPSERVER_H
