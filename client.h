#pragma once

#include "chartmanager.h"
#include <QtWidgets>
#include <QUdpSocket>
#include <QTcpSocket>

class Client : public QWidget
{
    Q_OBJECT
private:
    ChartManager* chartManager;
    QTextEdit* receivedCircuitData;

    int udpPort;
    QUdpSocket* udpSocket;

    int tcpPort;
    QTcpSocket* tcpSocket;
    quint16 nextBlockSize;
    QLineEdit *windowInput;
    QTextEdit *serverResponseText;
    void parseStringData(QString stringData);
    // void parseReceivedData(QList<QString> stringData);
    // void parseReceivedAnalysis(QList<QString> stringData);

public:
    Client(const QString& strHost, int tcpPort, int udpPort, QWidget* pwgt = 0);

private slots:
    void slotProcessDatagrams();
    void slotReadyRead();
    void slotError(QAbstractSocket::SocketError error);
    void slotSendToServer();
    void slotConnected();

signals:
    void signalReceivedData(xyzCircuitData data);
    void signalReceivedAnalysis(xyzAnalysisResult analysis);
};
