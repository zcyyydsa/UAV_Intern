#ifndef UDPHANDLE_H
#define UDPHANDLE_H

#include "include.h"
#include "SendThread.h"
extern QMap<int, QByteArray> mapSenddata;
extern int flag;
extern int flyflag1;
extern int flyflag2;
extern int flyflag3;

extern QMap<int, FLY_SET_DATA> mapSetFlyData;

class udphandle : public QObject
{
    Q_OBJECT
public:
    explicit udphandle(QObject *parent = nullptr);
    ~udphandle();
    void handledata(QByteArray datagram);
    void Packaging(int id, int x, int y, int z, int yaw);
    void PackagingVision();
signals:
    void result(int);
    void sigjsondata(int);
    void landingid(int);
private:
    int idx[5];
    int idy[5];
    int idz[5];
    int idw[5];
};
#endif // UDPHANDLE_H
