#include "udphandle.h"
QMutex mutexjson;
udphandle::udphandle(QObject *parent) : QObject(parent)
{

}

udphandle::~udphandle()
{

}

void udphandle::handledata(QByteArray datagram)
{
    memset(&idx, 0, sizeof(idx));
    memset(&idy, 0, sizeof(idy));
    memset(&idz, 0, sizeof(idz));
    memset(&idw, 0, sizeof(idw));

    QString json_str(datagram);
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(json_str.toUtf8(), &jsonError);
    if (jsonError.error != QJsonParseError::NoError && !doc.isNull())
    {
        qDebug() << "Json格式错误！" << jsonError.error;
        return;
    }
    if (!doc.isNull())
    {
        if (doc.isObject())
        {
            QJsonObject object = doc.object();
            if (object.contains("landing"))
            {
                QJsonValue value = object.value("landing");
                if (value.isDouble())
                {
                    qDebug()<<"landingid"<< value.toInt();
                    emit landingid(value.toInt());
                    return;
                }
            }
        }
    }
    if(flag == 1)
    {
        if (!doc.isNull())
        {
            if (doc.isObject())
            {
                QJsonObject object = doc.object();
                if (object.contains("4"))      //小车坐标
                {
                    QJsonValue id = object.value("4");
                    if (id.isObject())
                    {
                        QJsonObject obj = id.toObject();
                        if (obj.contains("x"))
                        {
                            QJsonValue value = obj.value("x");
                            if (value.isDouble())
                            {
                                idx[3] = value.toDouble();
                                //qDebug() << "x : " << idx[3];
                            }
                        }
                        if (obj.contains("y"))
                        {
                            QJsonValue value = obj.value("y");
                            if (value.isDouble())
                            {
                                idy[3] = value.toDouble();
                                //qDebug() << "y : " << idy[3];
                            }
                        }
                        if (obj.contains("z"))
                        {
                            QJsonValue value = obj.value("z");
                            if (value.isDouble())
                            {
                                idz[3] = value.toDouble();
                                //qDebug() << "z : " << idz[3];
                            }
                        }
                        if (obj.contains("yaw"))
                        {
                            QJsonValue value = obj.value("yaw");
                            if (value.isDouble())
                            {
                                idw[3] = value.toDouble();
                                //qDebug() << "yaw : " << idw[3];
                            }
                        }
                    }
                    Packaging(4, idx[3], idy[3], idz[3], idw[3]);
                }
                if (object.contains("5"))        //小车坐标
                {
                    QJsonValue id = object.value("5");
                    if (id.isObject())
                    {
                        QJsonObject obj = id.toObject();
                        if (obj.contains("x"))
                        {
                            QJsonValue value = obj.value("x");
                            if (value.isDouble())
                            {
                                idx[4] = value.toInt();
                                //qDebug() << "x : " << idx[4];
                            }
                        }
                        if (obj.contains("y"))
                        {
                            QJsonValue value = obj.value("y");
                            if (value.isDouble())
                            {
                                idy[4] = value.toInt();
                                //qDebug() << "y : " << idx[4];
                            }
                        }
                        if (obj.contains("z"))
                        {
                            QJsonValue value = obj.value("z");
                            if (value.isDouble())
                            {
                                idz[4] = value.toInt();
                                //qDebug() << "z : " << idz[4];
                            }
                        }
                        if (obj.contains("yaw"))
                        {
                            QJsonValue value = obj.value("yaw");
                            if (value.isDouble())
                            {
                                idw[4] = value.toInt();
                                //qDebug() << "yaw : " << idw[4];
                            }
                        }
                    }
                    Packaging(5, idx[4], idy[4], idz[4], idw[4]);
                }

            }
        }
    }
    else if(flag == 2)
    {
        if (!doc.isNull())
        {
            if (doc.isObject())
            {
                QJsonObject object = doc.object();
                if (object.contains("1"))      //飞机坐标
                {
                    QJsonValue id = object.value("1");
                    if (id.isObject())
                    {
                        QJsonObject obj = id.toObject();
                        if (obj.contains("x"))
                        {
                            QJsonValue value = obj.value("x");
                            if (value.isDouble())
                            {
                                idx[0] = value.toDouble();
                                //qDebug() << "x : " << idx[0];
                            }
                        }
                        if (obj.contains("y"))
                        {
                            QJsonValue value = obj.value("y");
                            if (value.isDouble())
                            {
                                idy[0] = value.toDouble();
                                //qDebug() << "y : " << idy[0];
                            }
                        }
                        if (obj.contains("z"))
                        {
                            QJsonValue value = obj.value("z");
                            if (value.isDouble())
                            {
                                idz[0] = value.toDouble();
                                //qDebug() << "z : " << idz[0];
                            }
                        }
                        if (obj.contains("yaw"))
                        {
                            QJsonValue value = obj.value("yaw");
                            if (value.isDouble())
                            {
                                idw[0] = value.toDouble();
                                //qDebug() << "yaw : " << idw[0];
                            }
                        }
                    }
                }
                if (object.contains("2"))      //飞机坐标
                {
                    QJsonValue id = object.value("2");
                    if (id.isObject())
                    {
                        QJsonObject obj = id.toObject();
                        if (obj.contains("x"))
                        {
                            QJsonValue value = obj.value("x");
                            if (value.isDouble())
                            {
                                idx[1] = value.toDouble();
                                //qDebug() << "x : " << idx[1];
                            }
                        }
                        if (obj.contains("y"))
                        {
                            QJsonValue value = obj.value("y");
                            if (value.isDouble())
                            {
                                idy[1] = value.toDouble();
                                //qDebug() << "y : " << idy[1];
                            }
                        }
                        if (obj.contains("z"))
                        {
                            QJsonValue value = obj.value("z");
                            if (value.isDouble())
                            {
                                idz[1] = value.toDouble();
                                //qDebug() << "z : " << idz[1];
                            }
                        }
                        if (obj.contains("yaw"))
                        {
                            QJsonValue value = obj.value("yaw");
                            if (value.isDouble())
                            {
                                idw[1] = value.toDouble();
                                //qDebug() << "yaw : " << idw[1];
                            }
                        }
                    }
                }
                if (object.contains("3"))      //飞机坐标
                {
                    QJsonValue id = object.value("3");
                    if (id.isObject())
                    {
                        QJsonObject obj = id.toObject();
                        if (obj.contains("x"))
                        {
                            QJsonValue value = obj.value("x");
                            if (value.isDouble())
                            {
                                idx[2] = value.toDouble();
                                //qDebug() << "x : " << idx[2];
                            }
                        }
                        if (obj.contains("y"))
                        {
                            QJsonValue value = obj.value("y");
                            if (value.isDouble())
                            {
                                idy[2] = value.toDouble();
                                //qDebug() << "y : " << idy[2];
                            }
                        }
                        if (obj.contains("z"))
                        {
                            QJsonValue value = obj.value("z");
                            if (value.isDouble())
                            {
                                idz[2] = value.toDouble();
                                //qDebug() << "z : " << idz[2];
                            }
                        }
                        if (obj.contains("yaw"))
                        {
                            QJsonValue value = obj.value("yaw");
                            if (value.isDouble())
                            {
                                idw[2] = value.toDouble();
                                //qDebug() << "yaw : " << idw[2];
                            }
                        }
                    }
                }
                if (object.contains("4"))      //小车坐标
                {
                    QJsonValue id = object.value("4");
                    if (id.isObject())
                    {
                        QJsonObject obj = id.toObject();
                        if (obj.contains("x"))
                        {
                            QJsonValue value = obj.value("x");
                            if (value.isDouble())
                            {
                                idx[3] = value.toDouble();
                                //qDebug() << "x : " << idx[3];
                            }
                        }
                        if (obj.contains("y"))
                        {
                            QJsonValue value = obj.value("y");
                            if (value.isDouble())
                            {
                                idy[3] = value.toDouble();
                                //qDebug() << "y : " << idy[3];
                            }
                        }
                        if (obj.contains("z"))
                        {
                            QJsonValue value = obj.value("z");
                            if (value.isDouble())
                            {
                                idz[3] = value.toDouble();
                                //qDebug() << "z : " << idz[3];
                            }
                        }
                        if (obj.contains("yaw"))
                        {
                            QJsonValue value = obj.value("yaw");
                            if (value.isDouble())
                            {
                                idw[3] = value.toDouble();
                                //qDebug() << "yaw : " << idw[3];
                            }
                        }
                    }
                }
                if (object.contains("5"))      //小车坐标
                {
                    QJsonValue id = object.value("5");
                    if (id.isObject())
                    {
                        QJsonObject obj = id.toObject();
                        if (obj.contains("x"))
                        {
                            QJsonValue value = obj.value("x");
                            if (value.isDouble())
                            {
                                idx[4] = value.toInt();
                                //qDebug() << "x : " << idx[4];
                            }
                        }
                        if (obj.contains("y"))
                        {
                            QJsonValue value = obj.value("y");
                            if (value.isDouble())
                            {
                                idy[4] = value.toInt();
                                //qDebug() << "y : " << idx[4];
                            }
                        }
                        if (obj.contains("z"))
                        {
                            QJsonValue value = obj.value("z");
                            if (value.isDouble())
                            {
                                idz[4] = value.toInt();
                                //qDebug() << "z : " << idz[4];
                            }
                        }
                        if (obj.contains("yaw"))
                        {
                            QJsonValue value = obj.value("yaw");
                            if (value.isDouble())
                            {
                                idw[4] = value.toInt();
                                //qDebug() << "yaw : " << idw[4];
                            }
                        }
                    }

                }
            }
            PackagingVision();
        }
    }
    return;
}


void udphandle::Packaging(int id, int x, int y, int z, int yaw)      //打包
{
    //将map数据取出
    QMap<int, FLY_SET_DATA>::const_iterator iter = mapSetFlyData.constBegin();
    while (iter != mapSetFlyData.constEnd())
    {
        if(iter.value().carnum.toInt() == id)
        {
            int flyid = iter.key();
            if(flyflag1 == 1 && flyid == 1)
                flyid = 1;
            else if(flyflag2 == 1 && flyid == 2)
                flyid = 2;
            else if(flyflag3 == 1 && flyid == 3)
                flyid = 3;
            else
            {
                ++iter;
                continue;
            }
           qDebug()  << "flyid = "<< flyid;
           uint8_t sbuf[512] = {0};
           hgprotocol_message_t  msg;
           hgprotocol_lcommand_t flydata;
           memset(&flydata, 0, sizeof(hgprotocol_lcommand_t));
           flydata.cmd = MAV_CMD_GO;
           flydata.start_id = flyid;
           flydata.end_id = flyid;
           if(iter.value().direction == QString("0"))//前
           {
               flydata.param[0] = x* 0.1 + iter.value().radius.toInt();
               flydata.param[1] = -y* 0.1;
           }
           else if(iter.value().direction == QString("1"))//右前
           {
               flydata.param[0] = x* 0.1 + iter.value().radius.toInt()*cos(45/57.3);
               flydata.param[1] = -y* 0.1 + iter.value().radius.toInt()*sin(45/57.3);
           }
           else if(iter.value().direction == QString("2"))//右
           {
               flydata.param[0] = x * 0.1;
               flydata.param[1] = -y * 0.1 + iter.value().radius.toInt();
           }
           else if(iter.value().direction == QString("3"))//右后
           {
               flydata.param[0] = x * 0.1 - iter.value().radius.toInt()*cos(45/57.3);      //
               flydata.param[1] = -y * 0.1 + iter.value().radius.toInt()*sin(45/57.3);
           }
           else if(iter.value().direction == QString("4"))//后
           {
               flydata.param[0] = x * 0.1 - iter.value().radius.toInt();      //
               flydata.param[1] = -y * 0.1;
           }
           else if(iter.value().direction == QString("5"))//左后
           {
               flydata.param[0] = x * 0.1- iter.value().radius.toInt()*cos(45/57.3);  //
               flydata.param[1] = -y * 0.1 - iter.value().radius.toInt()*sin(45/57.3);   //
           }
           else if(iter.value().direction == QString("6"))//左
           {
               flydata.param[0] = x * 0.1;
               flydata.param[1] = -y * 0.1 - iter.value().radius.toInt();          //
           }
           else if(iter.value().direction == QString("7"))//左前
           {
               flydata.param[0] = x * 0.1 + iter.value().radius.toInt()*cos(45/57.3);
               flydata.param[1] = -y * 0.1 - iter.value().radius.toInt()*sin(45/57.3);
           }
           else if(iter.value().direction == QString("8"))//中
           {
               flydata.param[0] = x * 0.1;
               flydata.param[1] = -y * 0.1;
           }
           else
           {
               ++iter;
               continue;
           }

           flydata.param[2] = - iter.value().height.toInt();           // -500 - 500
           flydata.param[3] = iter.value().speed.toInt();           //10-100(cm/s)
           flydata.param[4] = - yaw;
           hgprotocol_lcommand_encode(0, 0, &msg, &flydata);
           int size = hgprotocol_msg_to_send_buffer(sbuf, &msg);
           QByteArray flyBuf;
           flyBuf.clear();
           flyBuf.resize(size);
           memcpy(flyBuf.data(), sbuf, size);

           mutexjson.lock();
           mapSenddata[flyid] = flyBuf;
           mutexjson.unlock();

           emit sigjsondata(flyid);
        }
        ++iter;
    }
    return;
}


void udphandle::PackagingVision()
{
    QDateTime timeDate = QDateTime::currentDateTime();  // 获取当前时间
    int timeStr = timeDate .toTime_t();   				// 将当前时间转为时间戳
    uint8_t sbuf[512] = {0};
    //将map数据取出
    FLY_SET_DATA fly[3]; // 三台无人机各自的参数设置，radius, carnum, heigh, direction, speed, covering
    int flyid[3];
    int sum = 0;
    QByteArray flyBuf;
    flyBuf.clear();
    memset(&flyid, 0, sizeof(flyid));
    hgprotocol_message_t  msg;
    QMap<int, FLY_SET_DATA>::const_iterator iter = mapSetFlyData.constBegin();
    while (iter != mapSetFlyData.constEnd())
    {
        flyid[sum] = iter.key();
        fly[sum] = iter.value();
        hgprotocol_swarm_track_t flydata;
        memset(&flydata, 0, sizeof(hgprotocol_swarm_track_t));
        flydata.time_stamp = timeStr;
        flydata.fun_id = 1;
        flydata.swarm_state = START_SWARM_TRACK;
        for(int i = 0; i < 3; i++)
        {
            flydata.pos[i*5] = idx[i]* 0.1;
            flydata.pos[i*5+1] = idy[i]* 0.1;
            flydata.pos[i*5+2] = idz[i]* 0.1;
            flydata.pos[i*5+3] = idw[i]* 0.1;
            flydata.pos[i*5+4] = fly[i].carnum.toFloat();
            flydata.is_candidate[i] = fly[i].covering.toInt();
            flydata.mav_target[i] = fly[i].carnum.toFloat();
            flydata.mav_target[3+i] = fly[i].direction.toFloat();
            flydata.track_dist[i] = fly[i].radius.toInt();
            flydata.track_dist[3+i] = fly[i].height.toInt();
        }
        // 小车0
        flydata.vehicle0[0] = idx[3]* 0.1;
        flydata.vehicle0[1] = idy[3]* 0.1;
        flydata.vehicle0[2] = idz[3]* 0.1;
        flydata.vehicle0[3] = idw[3]* 0.1;
        // 小车1
        flydata.vehicle1[0] = idx[4]* 0.1;
        flydata.vehicle1[1] = idy[4]* 0.1;
        flydata.vehicle1[2] = idz[4]* 0.1;
        flydata.vehicle1[3] = idw[4]* 0.1;

        hgprotocol_swarm_track_encode(0, 0, &msg, &flydata);
        int size = hgprotocol_msg_to_send_buffer(sbuf, &msg);

        flyBuf.resize(size);
        memcpy(flyBuf.data(), sbuf, size);
        mutexjson.lock();
        mapSenddata.insert(flyid[sum], flyBuf);
        mutexjson.unlock();
        emit sigjsondata(flyid[sum]);
        ++iter;
        ++sum;
    }   
    return;
}
