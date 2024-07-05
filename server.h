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
    // DataReceiver *dataReceiver;
    int udpPort;
    QUdpSocket* udpSocket;
    QTextEdit* receivedDataText;
    QTextEdit* sentDataText;
    QQueue<xyzCircuitData> dataToSendQueue;
    QQueue<QString> stringsToSendQueue;

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
    void slotAnalysisToSendAdded(xyzAnalysisResult analysis);
    void slotSendDatagram();
signals:
    void signalWindowSizeChanged(int newSize);
    void signalTimeToClearChanged(int newTime);
    void signalAnalysisToggle(QString analysisType);
};

// #endif // UDPSERVER_H
