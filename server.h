#pragma once

#include <QtWidgets>
#include <QUdpSocket>
#include <QTcpServer>
#include "P7_Telemetry.h"
#include "P7_Trace.h"
#include "dataanalyzer.h"
#include "dataprocessor.h"

class Server : public QWidget
{
    Q_OBJECT
private:
    IP7_Trace *p7Trace;
    IP7_Trace::hModule moduleName;
    IP7_Telemetry *p7Telemetry;
    tUINT16 l_wCpuId = 0;
    tUINT16 l_wMemId = 0;
    tDOUBLE l_dbCPU = 0;
    tDOUBLE l_dbMem = 0;

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
