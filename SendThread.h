#ifndef SENDTHREAD_H
#define SENDTHREAD_H

#include "include.h"

extern QMap<int, QByteArray> mapSenddata;

class sendthread : public QObject
{
    Q_OBJECT
public:
    explicit sendthread(QObject *parent = nullptr);
    ~sendthread() override;
public slots:
    void senddata(int id);
signals:
    void OpenWrite();
    void CriticalNofly();
    void writeDatas(QTcpSocket *tcp, QByteArray data);
};


#endif // SENDTHREAD_H
