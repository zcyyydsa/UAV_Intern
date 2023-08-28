#include "udpstates.h"

udpstates::udpstates(QObject *parent) : QObject(parent)
{
    //UDP组
    sender  = new QUdpSocket(this);// 在未来发送数据的时候，明确ip和port即可
    reciver = new QUdpSocket(this);// 需要绑定一下接受数据的ip和port，根据情况不同，还需要加入广播组
    sender->bind(8888);

    reciver->bind(QHostAddress::Any, 8900);
    //客户端发送数据后触发readyRead
    //connect(reciver, SIGNAL(readyRead()),this, SLOT(recvdata()));
    connect(reciver, &QUdpSocket::readyRead, this, &udpstates::recvdata);

}

udpstates::~udpstates()
{
    delete sender;
    delete reciver;
}

// 发送ip
void udpstates::startDateTime(QString pcip)
{
    //QString pcip = "192.168.43.255";
    QString str = "IP_FOUND";
    int ret = sender->writeDatagram(str.toLocal8Bit().data(), str.length(), QHostAddress(pcip), 8888);
    emit result(ret);
}

void udpstates::recvdata()
{
    //只要缓冲区有数据，那么就不断的读取然后处理
    QByteArray datagram;
    while (reciver->hasPendingDatagrams())
    {
        //把缓冲区的大小重设为待读取的数据的大小
        datagram.resize(reciver->pendingDatagramSize());

        //datagram.data()是把QByteArray转为char *
        //sender代表发送端的IP地址，senderPort代表发送端的端口
        reciver->readDatagram(datagram.data(), datagram.size(), &recvip, &port);
        //qDebug() << datagram.data();
        emit jsondata(datagram);    //发送数据解析json
    }
}

