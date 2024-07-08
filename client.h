#pragma once


#include "chartmanager.h"
#include <QtWidgets>
#include <QUdpSocket>
#include <QTcpSocket>
#include "P7_Trace.h"
#include "P7_Telemetry.h"
#include "circuitconfiguratorwidget.h"
class Client : public QWidget
{
    Q_OBJECT
private:
    IP7_Trace *p7Trace;
    IP7_Telemetry *p7Telemetry;
    IP7_Trace::hModule moduleName;

    ChartManager* chartManager;
    QTextEdit* receivedCircuitData;

    int udpPort;
    QUdpSocket* udpSocket;

    int tcpPort;
    QTcpSocket* tcpSocket;
    quint16 nextBlockSize;
    QLineEdit *windowInput;
    QLineEdit *timeToClearInput;
    QTextEdit *serverResponseText;
    void parseStringData(QString stringData);
    void sendToTcpServer(QString messageType, QString message);


    CircuitConfiguratorWidget *aConfig;
    CircuitConfiguratorWidget *gConfig;
    CircuitConfiguratorWidget *mConfig;

public:
    Client(const QString& strHost, int tcpPort, int udpPort, QWidget* pwgt = 0);

private slots:
    void slotProcessDatagrams();
    void slotReadyRead();
    void slotError(QAbstractSocket::SocketError error);
    void slotServerChangeWindowSize();
    void slotServerToggleWindow();
    void slotServerChangeTimeToCleanup();
    void slotConnected();
    void slotConfigChanged(cConfig config);

signals:
    void signalReceivedData(xyzCircuitData data);
    void signalReceivedAnalysis(xyzAnalysisResult analysis);

};
