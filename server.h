#pragma once

#include <QtWidgets>
#include <QUdpSocket>
#include <QTcpServer>
#include "CircuitConfiguration.h"
#include "P7_Trace.h"
#include "dataanalyzer.h"
#include "dataprocessor.h"

class Server : public QWidget
{
    Q_OBJECT
private:
    IP7_Trace *p7Trace;
    IP7_Trace::hModule moduleName;


    DataProcessor *dataProcessor;
    DataAnalyzer *dataAnalyzer;
    // DataReceiver *dataReceiver;
    int udpPort;
    QUdpSocket* udpDataSocket;
    QUdpSocket* udpAnalysisSocket;

    QTextEdit* receivedDataText;
    QTextEdit* sentDataText;
    QQueue<xyzCircuitData> cDataToSendQueue;
    QQueue<xyzAnalysisResult> cAnalysisToSendQueue;

    QList<cConfig> configsReceived;

    int tcpPort;
    bool clientConnected = false;
    QTcpServer* tcpServer;
    QTcpSocket* clientSocket;
    QTextEdit *clientResponseText;
    quint16 nextBlockSize;
    void sendToClient(QTcpSocket* socket, const QString &str);
    bool processClientResponse(QString messageType, QString message);
    void sendData();
    void sendAnalysis();
public:
    Server(int tcpPort, int udpPort, QWidget* pwgt = 0);

public slots:
    void slotStringReceived(QString string);
    void slotNewConnection();
    void slotReadClient();
    void slotDataToSendAdded(xyzCircuitData data);
    void slotAnalysisToSendAdded(xyzAnalysisResult analysis);
    void slotSendDatagram();
    void slotSendCircuitMessageToClient(QString string);
    void slotSendDeltaTime(xyzAnalysisResult analysis);
signals:
    void signalWindowSizeChanged(int newSize);
    void signalTimeToClearChanged(int newTime);
    void signalAnalysisToggle(QString analysisType);
    void signalConfigReceived(QList<cConfig> configsReceived);
};

// #endif // UDPSERVER_H
