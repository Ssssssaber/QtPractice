#ifndef CLIENT_H
#define CLIENT_H



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

    QHostAddress serverAddress;

    ChartManager* chartManager;
    // QTextEdit* receivedCircuitData;

    int udpPort;
    QUdpSocket* udpDataSocket;
    QUdpSocket* udpAnalysisSocket;

    int tcpPort;
    QTcpSocket* tcpSocket;
    quint16 nextBlockSize;
    QLineEdit *windowInput;
    QLineEdit *timeToClearInput;
    QTextEdit *serverResponseText;
    void parseStringData(QString stringData);
    void sendToTcpServer(QString messageType, QString message);
    void processServerResponse(QString message);

    QElapsedTimer receiveTime;
    qint64 lastReceivedTime;

    CircuitConfiguratorWidget *aConfig;
    CircuitConfiguratorWidget *gConfig;
    CircuitConfiguratorWidget *mConfig;

    QList<xyzCircuitData> statData;
    QLabel *threadLabel;
    QLabel *deltaLabel;
    int currentThread = 0;

    QList<QString> configStrings;


public:
    Client(int tcpPort = 2323, int udpPort = 2424, QHostAddress serverAddress = QHostAddress::LocalHost, QWidget* pwgt = 0);

private slots:
    void slotProcessDatagram();
    void slotReadyRead();
    void slotError(QAbstractSocket::SocketError error);
    void slotServerChangeWindowSize();
    void slotServerToggleWindow();
    void slotServerChangeTimeToCleanup();
    void slotConnected();
    void slotConfigChanged(cConfig config);
    void slotUpdateThreadInfo();

signals:
    void signalReceivedData(xyzCircuitData data);
    void signalReceivedAnalysis(xyzAnalysisResult analysis);

};
#endif
