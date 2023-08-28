#include "flystates.h"
#include "ui_flystates.h"
#include "mainwindow.h"
#include "flystates.h"


int client_num = 0;
QMutex datamutex;

FlyStates::FlyStates(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FlyStates)
{
    ui->setupUi(this);
}

FlyStates::~FlyStates()
{
    delete ui;
}


mythread::mythread(QObject *parent): QObject(parent)
{
    server = new QTcpServer(this);
    m_socket = new QTcpSocket(this);
    if(!server->listen(QHostAddress::Any, 8888))            //监听
    {
        //若出错，则输出错误信息
        qDebug()<<server->errorString();
        return;
    }
    connect(server, &QTcpServer::newConnection, this, &mythread::New_Connect);        //连接信号槽，当有新连接进来时

    //设置离线定时器
    r_Timer = new QTimer(this);
    connect(r_Timer, &QTimer::timeout, this, [=]()
    {
        qint64 millisecs = QDateTime::currentDateTime().toMSecsSinceEpoch();    //取当前时间
        QMap<int, qint64>::iterator itt;
        if(mapTimeInfo.isEmpty())
            return;
        for(itt = mapTimeInfo.begin(); itt!= mapTimeInfo.end(); )
        {
            if(millisecs - itt.value() > 5000)
            {
                //QMutexLocker lock(&datamutex);
                datamutex.lock();
                mapRobotInfo.remove(itt.key());
                mapTcpsocket.remove(itt.key());
                trueRobotInfo.remove(itt.key());
                itt = mapTimeInfo.erase(itt);
                datamutex.unlock();
            }
            else
            {
//                datamutex.lock();
//                itt = mapTimeInfo.erase(itt);
//                datamutex.unlock();
                itt++;
            }
        }

    });
    r_Timer->start(1500);
}


mythread::~mythread()
{
    m_socket->close();
    server->close();
    delete m_socket;
    delete server;
    delete r_Timer;
}

void mythread::ppty()
{
    emit display();
}


void mythread::New_Connect()
{
    //QString ip = ui->ip->text();
    //unsigned short port = ui->port->text().toUShort();
    //m_socket->connectToHost(QHostAddress(ip), port);      //连接端口

    //获取客户端连接
    m_socket = server->nextPendingConnection(); //取出建立好连接的套接字
    TcpSockets_List.append(m_socket);   //把新进来的客户端存入列表
    emit sendArrys(m_socket);
    QObject::connect(m_socket, &QTcpSocket::readyRead, this, &mythread::ReadData);
    QObject::connect(m_socket, &QTcpSocket::disconnected, this, &mythread::Disconnected); //当有socket的连接断开
    client_num++;
}

void mythread::Disconnected()
{
    //有客户端断开连接
    for (int i = 0; i < TcpSockets_List.count(); i++)
    {
        QTcpSocket* item = TcpSockets_List.at(i);
        //如果有客户端断开连接,将列表中的套接字删除
        if (item->state() == QAbstractSocket::UnconnectedState) // 判断socket断开连接的逻辑
        {
            TcpSockets_List.removeAt(i);
            return;
        }
    }
}

void mythread::handle_msg(QTcpSocket *tcp, hgprotocol_message_t* msg)
{
    switch(msg->msgid)
    {
    case HGPROTOCOL_MSG_ID_LACK:
    {
        hgprotocol_lack_t ack;
        hgprotocol_lack_decode(msg, &ack);
        //qDebug()<<"cmd:"<<ack.cmd ;
        if(ack.result == 0)
            ;//qDebug() << "成功LACK";
        else
            ;//qDebug() << "失败LACK";
        break;
    }
    case HGPROTOCOL_MSG_ID_LACK_EXTEND:
    {
        hgprotocol_lack_extend_t ack;
        hgprotocol_lack_extend_decode(msg, &ack);
        if(ack.result == 0)
            qDebug() << "成功EXTEND";
        else
             qDebug() << "失败EXTEND";
        break;
    }
    case HGPROTOCOL_MSG_ID_LFILEINFO:
    {
        hgprotocol_lfileinfo_t file;
        hgprotocol_lfileinfo_decode(msg, &file);
        break;
    }
    case HGPROTOCOL_MSG_ID_DRONE_STATS:    //心跳
    {
        hgprotocol_drone_stats_t dst;
        hgprotocol_drone_stats_decode(msg, &dst);
        qDebug()<<"heartbeat id:"<<dst.id ;

        QStringList bufflist;
        bufflist.append(u8"编号: " + QString::number(dst.id));
        bufflist.append(u8"电量: " + QString::number(dst.battery));
        bufflist.append(u8"yaw角: " + QString::number(dst.yaw));
        bufflist.append(u8"X轴: " + QString::number(dst.x));
        bufflist.append(u8"Y轴: " + QString::number(dst.y));
        bufflist.append(u8"Z轴: " + QString::number(dst.z));
        bufflist.append(u8"—————————————————— ");


        //将id和tcp存入map中
        QMap<int, QTcpSocket*>::iterator iti;
        iti = mapTcpsocket.find(dst.id);
        if (iti != mapTcpsocket.end())    //找到数据
        {
            ;//qDebug()<<"更新Info";
            mapTcpsocket[dst.id] = tcp;
        }
        else
        {
            qDebug()<<"新增Info";
            mapTcpsocket.insert(dst.id, tcp);
        }

        //将原始数据存入map中
        QMap<int, hgprotocol_drone_stats_t>::iterator its;
        its = trueRobotInfo.find(dst.id);
        if (its != trueRobotInfo.end())    //找到数据
        {
            ;//qDebug()<<"更新Info";
            trueRobotInfo[dst.id] = dst;

        }
        else
        {
            qDebug()<<"新增Info";
            trueRobotInfo.insert(dst.id, dst);
        }

        //将时间数据存入map中
        QMap<int, qint64>::iterator itt;
        itt = mapTimeInfo.find(dst.id);
        qint64 millisecs = QDateTime::currentDateTime().toMSecsSinceEpoch();
        if (itt != mapTimeInfo.end())    //找到数据
        {
           mapTimeInfo[dst.id] = millisecs;
        }
        else
        {
            mapTimeInfo.insert(dst.id, millisecs);
        }

        //将解析始数据存入map中
        //bool isok = mapRobotInfo.contains("data.payload.drone_stat.id");
        QMap<int, QStringList>::iterator it;
        it = mapRobotInfo.find(dst.id);
        if (it != mapRobotInfo.end())    //找到数据
        {
            mapRobotInfo[dst.id] = bufflist;
            emit display();
        }
        else
        {
            mapRobotInfo.insert(dst.id, bufflist);
            emit display();
        }


        //for (it = mapRobotInfo.begin(); it != mapRobotInfo.end(); ++it )
        //{
        //    //qDebug() << it->first << ": " << it->second << endl;
        //    listcount.append(it.value());
        //}

        //listmodel->setStringList(bufflist); //为模型设置list，会导入list的内容
        //ui->listView->setModel(listmodel);

        //QModelIndex index =listmodel->index(listmodel->rowCount()-1,0);//获取最后一行
        //listmodel->setData(index,"new item",Qt::DisplayRole);//设置显示文字
        break;
    }
    default:
        break;
    }
    //进入事件循环
    //exec();
}


//每次刷新无人机即时的信息
void mythread::translateToMavlink(QTcpSocket *tcp, const QByteArray& array)
{ 
    //const char * str = array.constData();     //QByteArrar转char*
    //qDebug() << "有数据到来M";
    hgprotocol_message_t msg;
    hgprotocol_status_t r_status;
    uint8_t *buff = (uint8_t*)array.data();
    for (int i=0; i < array.size(); i++)
    {
        if (HGPROTOCOL_FRAMING_OK == hgprotocol_parse_char(0, buff[i], &msg, &r_status))
        {
            handle_msg(tcp, &msg);;
        }
    }
}


void mythread::ReadData()
{
    //QSerialPort dret;
    buf.clear();
    for(int i = 0; i < TcpSockets_List.count(); i++)
    {
        QTcpSocket *socketbuf = TcpSockets_List.at(i);
        buf = socketbuf->readAll();
        if(!buf.isEmpty())       //if(buf.length()!=0)   //如果检测到接收到的数据长度不为0，则代表是这个客户端发送的数据
            translateToMavlink(socketbuf, buf);
        else
            continue;
    }
}

void mythread::sendData(QTcpSocket *tcp, QByteArray data)
{
    qDebug() << "senddata";
    int iSize =  tcp->write(data);
    if(iSize == -1)
        emit CriticalEro();
    else
        emit CriticalSuc();
}

