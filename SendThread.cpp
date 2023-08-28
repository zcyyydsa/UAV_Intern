#include "SendThread.h"
#include "flystates.h"

QMutex s_mutexlock;

 sendthread::sendthread(QObject *parent)
     : QObject(parent)
 {

 }

 sendthread::~sendthread()
 {

 }


void sendthread::senddata(int id)
{
    if(!mapSenddata.isEmpty())
    {
        //找到发送数据
        qDebug()<< "找到发送数据" << id;
        QByteArray buf;
        s_mutexlock.lock();
        QMap<int, QByteArray>::iterator itdata = mapSenddata.begin();
        int id = itdata.key();
        buf = itdata.value();
        mapSenddata.remove(id);
        s_mutexlock.unlock();

        //找到发送的socket
        QTcpSocket *n_socket = nullptr;
        QMap<int, QTcpSocket*>::iterator itsct;
        itsct = mapTcpsocket.find(id);
        if (itsct != mapTcpsocket.end())    //找到发送的tcp
        {
            n_socket = mapTcpsocket[id];
        }
        else
        {
             //qDebug()<< "未找到socket";
            //emit CriticalNofly();
            //break;
            //continue;
        }
        if(n_socket != 0)
        {          
            emit writeDatas(n_socket, buf);
            qDebug()<< "发送socket";
        }
    }
}
