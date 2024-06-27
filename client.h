#pragma once

#include <QtWidgets>
#include <QUdpSocket>

class Client : public QTextEdit
{
    Q_OBJECT
private:
    QUdpSocket* m_udp;

public:
    Client(QWidget* pwgt = 0);

private slots:
    void slotProcessDatagrams();
};

// #endif // UDPCLIENT_H
