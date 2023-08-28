#ifndef FLYSTATES_H
#define FLYSTATES_H

#include "include.h"
#include "hg_protocol.h"


extern QList<QTcpSocket*> TcpSockets_List;  //存储连接到的socket
extern QMap<int, QStringList> mapRobotInfo;
extern QMap<int, qint64> mapTimeInfo;
extern QMap<int, QTcpSocket*> mapTcpsocket; //存储连接的ip和socket
extern QMap<int, hgprotocol_drone_stats_t> trueRobotInfo;
extern int client_num;

typedef unsigned long long uint64_t;

namespace Ui {
class FlyStates;
}


class FlyStates : public QDialog
{
    Q_OBJECT

public:
    explicit FlyStates(QWidget *parent = nullptr);
    ~FlyStates();
    //void ReadData();
    int state_machine;
private slots:
    //void translateToMavlink(QTcpSocket *tcp, const QByteArray& array);
signals:
    void auxsend();
    //void MySignal(mavlink_report_stats_t report_stats);
public:
    Ui::FlyStates *ui;
};


class mythread : public QObject
{
    Q_OBJECT
public:
    explicit mythread(QObject *parent = nullptr);
    ~mythread();
    //void recvnmu(int num);
    void handle_msg(QTcpSocket *tcp, hgprotocol_message_t* msg);
    FlyStates *uistates = new FlyStates;
    void ppty();

protected:
    void ReadData();
signals:
    void display();
    void sendArrys(QTcpSocket* temp);
    void CriticalSuc();
    void CriticalEro();
private:
    QByteArray buf;
    int state_machine;
    QTcpServer* server;
    QTcpSocket* m_socket;
    QTimer  *r_Timer;
public slots:
    void translateToMavlink(QTcpSocket *tcp, const QByteArray& array);
    void New_Connect();
    void Disconnected();
    void sendData(QTcpSocket *tcp, QByteArray data);

};


#endif // FLYSTATES_H
