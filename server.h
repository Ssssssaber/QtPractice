#pragma once

#include <QtWidgets>
#include <QUdpSocket>
#include <QTcpServer>
#include "CircuitConfiguration.h"
#include "P7_Trace.h"
#include "circuitmanager.h"

class DataProcessor;

class Server : public QWidget
{
    Q_OBJECT
private:
    IP7_Trace *p7Trace;
    IP7_Trace *p7TraceData;
    IP7_Trace::hModule moduleName;

    bool serverConnected;

    QHostAddress clientAddress;

    DataProcessor *dataProcessor;
    CircuitManager *circuitManger;
    QThread dataThread;
    // DataReceiver *dataReceiver;
    int udpPort;
    QUdpSocket* udpDataSocket;

    QTextEdit* receivedDataText;
    QTextEdit* sentDataText;

    QList<cConfig> configsReceived;

    int tcpPort;
    bool clientConnected = false;
    QTcpServer* tcpServer;
    QTcpSocket* clientSocket;
    QTextEdit *clientResponseText;
    quint16 nextBlockSize;
    void sendToClient(QTcpSocket* socket, const QString &str);
    bool processClientResponse(QString messageType, QString message);
public:
    bool isServerActive();
    Server(int tcpPort = 2323, int udpPort = 2424, QHostAddress hostAddress = QHostAddress::LocalHost, QWidget* pwgt = 0);
    ~Server();
public slots:
    void slotStringReceived(QString string);
    void slotNewConnection();
    void slotReadClient();
    void slotSendCircuitMessageToClient(QString string);
    void slotSendDeltaTime(xyzAnalysisResult analysis);
    void slotClrearSentData(int toSize);
signals:
    void signalWindowSizeChanged(int newSize);
    void signalStartProcessingData();
    void signalTimeToClearChanged(int newTime);
    void signalAnalysisToggle(QString analysisType, bool isActive);
    void signalConfigReceived(QList<cConfig> configsReceived);
};

// #endif // UDPSERVER_H
