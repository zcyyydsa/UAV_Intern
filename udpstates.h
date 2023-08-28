#ifndef UDPSTATES_H
#define UDPSTATES_H

#include "include.h"
#include "udphandle.h"

class udpstates : public QObject
{
    Q_OBJECT
public:
    explicit udpstates(QObject *parent = nullptr);
    ~udpstates();
    void startDateTime(QString pcip);
private:
    QUdpSocket* reciver;     //QUdpSocket对象,接收
    QUdpSocket* sender;      //QUdpSocket对象, 发送
    QHostAddress recvip;
    uint16_t port;

signals:
    void result(int);
    void jsondata(QByteArray datagram);
private slots:
    void recvdata();
};

#endif // UDPSTATES_H
