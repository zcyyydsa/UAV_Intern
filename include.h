#ifndef INCLUDE_H
#define INCLUDE_H

#include <QDialog>
#include <QThread>
#include <QMap>
#include <QTcpServer>
#include <QTcpSocket>
#include <stdint.h>
#include <QStringListModel>
#include <QStackedLayout>
#include <QComboBox>
#include <QMouseEvent>
#include <QUdpSocket>
#include <QTimer>
#include "hg_protocol.h"
#include <QObject>
#include <QVector>
#include <QMutex>
#include <QIODevice>
#include <QString>
#include <QDateTime>
#include <QMessageBox>
#include <QDebug>
#include <QTime>
#include <QInputDialog>
#include <QPointer>
#include <QProcess>
#include <QDir>
#include <QFileDialog>
#include <QTextCodec>
#include <QLabel>
#include <QHostAddress>
#include <windows.h>
#include <QCloseEvent>
#include <QPainter>
#include <math.h>
#include <stdio.h>
#include <iostream>
#include <QJsonDocument>
#include <QJsonObject>

typedef struct FLY_SET_DATA
{
    QString radius;
    QString carnum;
    QString height;
    QString direction;
    QString speed;
    QString covering;
}FLY_SET_DATA;


#endif // INCLUDE_H
