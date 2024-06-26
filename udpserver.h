#ifndef UDPSERVER_H
#define UDPSERVER_H

#include <QtWidgets>
#include <QUdpSocket>

class UdpServer : public QTextEdit
{
    Q_OBJECT
private:
    QUdpSocket* m_udp;
public:
    UdpServer(QWidget* pwgt = 0);
private slots:
    void slotSendDatagram();
};

#endif // UDPSERVER_H
