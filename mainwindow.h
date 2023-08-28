#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "flystates.h"
#include "SendThread.h"
#include "VideoThread.h"
#include "include.h"
#include "udpstates.h"
#include "paintarea.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


typedef enum MAV_SYS_SENSOR
{
    MAV_SYS_SENSOR_IMU1=0x01, //IMU1
    MAV_SYS_SENSOR_IMU2=0x02, //IMU2
    MAV_SYS_SENSOR_MAG=0x04, //磁罗盘1
    MAV_SYS_SENSOR_MAG2=0x08, //磁罗盘2
    MAV_SYS_SENSOR_BARO=0x10, //气压计
    MAV_SYS_SENSOR_FLASH=0x20, //SPI FLASH
    MAV_SYS_SENSOR_RTK=0x40, //RTK
    MAV_SYS_SENSOR_XBEE=0x80, //XBEE
    MAV_SYS_SENSOR_BATTERY=0x100, //智能电池
    MAV_SYS_SENSOR_WIFI=0x200, //WIFI模块
    MAV_SYS_SENSOR_XBEE_CONTROL=0x400, //上位机Xbee控制模块
    MAV_SYS_SENSOR_XBEE_2_4G=0x800, //2.4G XBEE
    MAV_SYS_SENSOR_GPS=0x1000, //备份GPS
} MAV_SYS_SENSOR;


typedef enum MAV_SYS_RTK
{
    MAV_RTK_STATUS_NO_LOCATION=0,//无定位
    MAV_RTK_STATUS_3D_FIX=1,//3D定位
    MAV_RTK_STATUS_FLOAT=2,//float定位
    MAV_RTK_STATUS_FIX=3,//fix定位
} MAV_SYS_RTK;


typedef enum MAV_SYS_IMU
{
    MAV_IMU_STATUS_GYRO_CAL=0x01, //陀螺仪校准
    MAV_IMU_STATUS_ACCEL_CAL=0x02, //加速度计校准
    MAV_IMU_STATUS_HEALTHY=0x04, //IMU健康
    MAV_IMU_STATUS_USED=0x08, //用于飞行
    MAV_IMU_STATUS_ABNORMAL=0x10, //数据异常
} MAV_SYS_IMU;


typedef enum MAV_SYS_MAG
{
    MAV_MAG_STATUS_CAL=0x01, //磁罗盘校准
    MAV_MAG_STATUS_HEALTHY=0x02, //磁罗盘健康
    MAV_MAG_STATUS_USED=0x04, //用于飞行
    MAV_MAG_STATUS_ABNORMAL=0x08, //磁罗盘总线异常
} MAV_SYS_MAG;


enum RTSP_PLAY_STATE
{
    RPS_IDLE,    //空闲
    RPS_PREPARE, //连接中
    RPS_RUNNING, //播放中
    RPS_PAUSE    //暂停中
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QList<QByteArray>  m_listUpGradeData;
    QString strPath;
    int senddata(const uint8_t *data, int length, int id);
    void closeEvent(QCloseEvent *);
    //void paintEvent(QPaintEvent *event);
    void DInitUDP();
    void DFlyunLock();
    void DFlyLock();
    void DTakeOff();
    void DLanding();
    void DFlyReturn();
    void DSetID();
    void DSetAngle();
    void DCloGo();
    void DCouGo();
    void DRoot();
    void clean();
    void SetRobotTableProperty();
    void slotDisplayPlaneInfo(int row, int column);
    void DLanding2();
    void DTakeOff2();
    void DTakeOff3();
    void DFlyReturn2();
    void DFlyUp();
    void DFlyDown();
    void DFlyBack();
    void DFlyFront();
    void DFlyLeft();
    void DFlyRight();
    void Distcp();
    void DPhotograph();
    void DVideo();
    void DToggleds(bool checked);
    void slotGetOneFrame();
    void DFlyStop();
    void DPTZ(int value);
    void DTiming();
    void slotFreshPlaneInfo();
    void Strprinft();
    void DStop();
    void FlyStart();
    void DTrack();
    void DUDPResult(int ret);
    void SetFly();
    void slotflyInfo(int row, int column);
    void DSave();
    void DCancel();
    void DLanding3();
    void DVision();
    void DConfirm1(bool checked);
    void DConfirm2(bool checked);
    void DConfirm3(bool checked);
    void DLanding4(int id);
private:
    Ui::MainWindow *ui;
    FlyStates *state = new FlyStates();
    videothread* v_thread = new videothread();
    QImage image;

    sendthread*  s_thread;
    QThread*     _sendThread = nullptr;
    mythread*    r_thread;
    QThread*     _recvThread = nullptr;
    udpstates*   u_thread;
    QThread*     _udpThread = nullptr;
    paintarea*   PaintArea = new paintarea(); 
    udphandle*   h_thread;
    QThread*     _udpHandle = nullptr;

    int     m_nCurrentRobot;
    int     m_nTableRowCount;
    int     m_TableColumeCount;
    bool    m_bPressed;
    int     rowbuff;
    int     colbuff;
    int     temporaryR = 0;
    int     temporaryC = 0;
    QTimer  *m_Timer;
    QPoint   m_point;
    QImage   m_Image;
    RTSP_PLAY_STATE m_kPlayState = RPS_IDLE;
    bool m_bHFlip = false;
    bool m_bVFlip = false;
    QString   m_strUrl;
    int       rowcolumn = -1;
    QStringList buffslist;
    QStringListModel *listmodel;
signals:
    void sigsenddata(int id);
    void test(QStringList);
    void pendis();
    void ipdata(QString);
};


class DrawingWidget : public QWidget
{
public:
    DrawingWidget(QWidget *parent = nullptr) : QWidget(parent)
    {
        // 设置窗口大小
        setFixedSize(400, 300);
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        QPainter painter(this);

        // 在指定区域内绘制矩形
        QRect rect1(50, 50, 200, 100);
        painter.drawRect(rect1);

        // 在指定区域内绘制椭圆
        QRect rect2(100, 150, 150, 150);
        painter.drawEllipse(rect2);

        QWidget::paintEvent(event); // 调用基类的绘制事件
    }
};

#endif // MAINWINDOW_H
