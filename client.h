#pragma once

#include "chartmanager.h"
#include <QtWidgets>
#include <QUdpSocket>

class Client : public QWidget
{
    Q_OBJECT
private:
    ChartManager* chartManager;
    QTextEdit* receivedData;
    QUdpSocket* m_udp;

public:
    Client(QWidget* pwgt = 0);

private slots:
    void slotProcessDatagrams();
    xyzCircuitData parseReceivedData(QString stringData);

signals:
    void signalClientReceivedData(xyzCircuitData data);
};
