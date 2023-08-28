#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_flystates.h"

QList<QTcpSocket*> TcpSockets_List;     //tcp list
QMap<int, QByteArray> mapSenddata;      //发送数据map
QMap<int, QTcpSocket*> mapTcpsocket;    //ip和对应的tcp
QMap<int, QStringList> mapRobotInfo;    //显示板内的list
QMap<int, hgprotocol_drone_stats_t> trueRobotInfo;   //原始数据，用于点击查看飞机属性
QMap<int, qint64> mapTimeInfo;          //飞机的心跳时间
QMap<int, FLY_SET_DATA> mapSetFlyData;          //飞机设置数据
QMutex mutexlock;
QImage img;
int flag = 0;
int flyflag1 = 0;
int flyflag2 = 0;
int flyflag3 = 0;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(QStringLiteral("客户端"));
    ui->comlabel->setText(u8"UDP未广播");
    connect(ui->openCOM, &QPushButton::clicked, this, &MainWindow::DInitUDP);
    connect(ui->FlyUnlock, &QPushButton::clicked, this, &MainWindow::DFlyunLock);
    connect(ui->Flylock, &QPushButton::clicked, this, &MainWindow::DFlyLock);
    connect(ui->Takeoff, &QPushButton::clicked, this, &MainWindow::DTakeOff);
    connect(ui->Landing, &QPushButton::clicked, this, &MainWindow::DLanding);
    connect(ui->Flyreturn, &QPushButton::clicked, this, &MainWindow::DFlyReturn);
    connect(ui->Setid, &QPushButton::clicked, this, &MainWindow::DSetID);
    connect(ui->Setangle, &QPushButton::clicked, this, &MainWindow::DSetAngle);
    connect(ui->Clogo, &QPushButton::clicked, this, &MainWindow::DCloGo);
    connect(ui->Cougo, &QPushButton::clicked, this, &MainWindow::DCouGo);
    connect(ui->Root, &QPushButton::clicked, this, &MainWindow::DRoot);
    connect(ui->clean, &QPushButton::clicked, this, &MainWindow::clean);
    //connect(ui->tableRobot, SIGNAL(cellClicked(int,int)), this, SLOT(slotDisplayPlaneInfo(int,int)));
    connect(ui->tableRobot, &QTableWidget::cellDoubleClicked, this, &MainWindow::slotDisplayPlaneInfo);
    connect(ui->horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(setValue(int)));
    connect(ui->flyWidget, &QTableWidget::cellClicked, this, &MainWindow::slotflyInfo);

    connect(ui->BtnLand, &QPushButton::clicked, this, &MainWindow::DLanding2);
    connect(ui->BtnTakeoff, &QPushButton::clicked, this, &MainWindow::DTakeOff2);
    connect(ui->BtnReturn, &QPushButton::clicked, this, &MainWindow::DFlyReturn2);
    //connect(state, &FlyStates::MySignal, this, &MainWindow::QueryState);
    connect(ui->BtnUp, &QPushButton::clicked, this, &MainWindow::DFlyUp);
    connect(ui->BtnDown, &QPushButton::clicked, this, &MainWindow::DFlyDown);
    connect(ui->BtnBack, &QPushButton::clicked, this, &MainWindow::DFlyBack);
    connect(ui->BtnFront, &QPushButton::clicked, this, &MainWindow::DFlyFront);
    connect(ui->BtnLeft, &QPushButton::clicked, this, &MainWindow::DFlyLeft);
    connect(ui->BtnRight, &QPushButton::clicked, this, &MainWindow::DFlyRight);
    connect(ui->BtnPicture, &QPushButton::clicked, this, &MainWindow::DPhotograph);
    connect(ui->BtnVideo, &QPushButton::clicked, this, &MainWindow::DVideo);
    connect(ui->BtnStop, &QPushButton::clicked, this, &MainWindow::DStop);
    connect(ui->BtnTiming, &QPushButton::clicked, this, &MainWindow::DTiming);
    connect(ui->BtnLanding, &QPushButton::clicked, this, &MainWindow::DLanding3);
    connect(ui->BtnConfirm, &QPushButton::clicked, this, &MainWindow::DTrack);
    connect(ui->BtnTakeoff2, &QPushButton::clicked, this, &MainWindow::DTakeOff3);


    //默认情况下checkable是不选中的，Button默认为触发按钮(trigger button)，按下去马上弹起来。
    //选中checkable后，Button变成切换按钮(toggle button)，可以有两种状态，即按下和弹起。
    //此时该按钮可以发射toggled(bool)信号，与槽函数setVisible(bool)结合，即可用于控件交替显示
    //没有选中checkable属性，就不可以触发toggled事件！
    connect(ui->Btntoggled, &QPushButton::toggled, this, &MainWindow::DToggleds);
    connect(ui->FlyStop, &QPushButton::clicked, this, &MainWindow::DFlyStop);
    connect(ui->PTZSlider, &QSlider::sliderMoved, this, &MainWindow::DPTZ);
    connect(ui->BtnSave, &QPushButton::clicked, this, &MainWindow::DSave);
    connect(ui->BtnCancel, &QPushButton::clicked, this, &MainWindow::DCancel);
    connect(ui->BtnVision, &QPushButton::clicked, this, &MainWindow::DVision);
    connect(ui->BtnConfirm1, &QPushButton::toggled, this, &MainWindow::DConfirm1);
    connect(ui->BtnConfirm2, &QPushButton::toggled, this, &MainWindow::DConfirm2);
    connect(ui->BtnConfirm3, &QPushButton::toggled, this, &MainWindow::DConfirm3);

    //ui->FlyUnlock->setDisabled(true);
    //ui->Flylock->setDisabled(true);
    ui->Takeoff->setDisabled(true);
    ui->Landing->setDisabled(true);
    ui->Flyreturn->setDisabled(true);
    ui->Setid->setDisabled(true);
    ui->Setangle->setDisabled(true);
    ui->Clogo->setDisabled(true);
    ui->Cougo->setDisabled(true);
    ui->FlyStop->setDisabled(true);
    ui->Btntoggled->setDisabled(true);

    //设置edit的限制
    ui->YLat->setValidator(new QDoubleValidator(-1000000.00, 1000000.00, 8));
    ui->XLat->setValidator(new QDoubleValidator(-1000000.00, 1000000.00, 8));
    ui->ZLat->setValidator(new QDoubleValidator(-1000000.00, 1000000.00, 8));
    ui->angleclo->setValidator(new QDoubleValidator(-1000000.00, 1000000.00, 8));
    ui->anglecou->setValidator(new QDoubleValidator(-1000000.00, 1000000.00, 8));
    ui->flyid->setValidator(new QIntValidator(-1000000, 1000000));
    ui->height->setValidator(new QDoubleValidator(0.00, 1000000.00, 8));
    ui->flystart->setValidator(new QIntValidator(0, 1000000));
    ui->flyend->setValidator(new QIntValidator(0, 1000000));

    //ui->EditRadius->setValidator(new QRegExpValidator(QRegExp("(([1-9]?|[1-9][0-9]?|[1-4][0-9][0-9]?)|500"), this));
    ui->EditRadius->setValidator(new QIntValidator(0, 150, this));
    ui->EditCar->setValidator(new QIntValidator(1, 3));
    ui->EditHeight->setValidator(new QIntValidator(50, 250, this));
    ui->EditSpeed->setValidator(new QIntValidator(50, 150, this));


    ui->tabWidget->installEventFilter(this);
    ui->port->setText("8888");
    ui->ip->setText("192.168.1.255");
    ui->lineEditUrl->setText("rtsp://:8554/live/ch0");
    //ui->lineEditUrl->setText("D:\\PV.mp4");
    ui->BtnConfirm->setStyleSheet("QPushButton{ background-color: rgb(0, 255, 255); border-radius: 3px; color: rgb(255, 255, 255);}");

    //QTabBar::tab {width:100px;height:40px;border:1px solid gray; border-bottom: none;border-top-left-radius: 4px;border-top-right-radius: 4px;background: white;}
    //ui->tab2->setStyleSheet("background-color:rgba(127,255,212,0.7);");
    //ui->tab->setStyleSheet("background-color:rgba(255,250,250,0.7);");

    ui->tabWidget->setStyleSheet("QTabWidget#tabWidget{background-color:rgb(0,0,0);}\
                                 QTabBar::tab{margin-right:5px;padding-left:50px;padding-right:50px;width:100px;height:40px;background-color:rgb(220,200,180);color:rgb(0,0,0);font:10pt '新宋体'}\
                                 QTabBar::tab::selected{background-color:rgb(0,100,200);color:rgb(255,0,0);font:10pt '新宋体'}");
                                 ui->tab->setStyleSheet("QWidget#tab{""background-color:#54FF9F;""border:1px solid #eeeeee;}");

    ui->tab2->setStyleSheet("QWidget#tab2{""background-color:#54FF9F;""border:1px solid #eeeeee;}");
    ui->tabWidget->setStyleSheet("QTabWidget::tab-bar { alignment: center; }");     //tab居中
    //ui->groupBox->setStyleSheet("background-color:rgb(220,200,180);");
    ui->groupBox2->setStyleSheet("background-color:rgb(255,255,255);");

    //ui->tabWidget->setStyleSheet("QTabBar::tab {width:100px;height:40px;border:1px solid gray; border-bottom: none;border-top-left-radius: 4px;border-top-right-radius: 4px;background: white;}");
    ui->Editnum->setStyleSheet("border: none;");    //去边框
    ui->Editnum->setFont(QFont("宋体",22));//设置字体和字体大小
    ui->Editnum2->setStyleSheet("border: none;");    //去边框
    ui->Editnum2->setFont(QFont("宋体",22));//设置字体和字体大小

    ui->EditValue->setStyleSheet("border: none;");
    ui->EditValue->setFont(QFont("微软雅黑",22));
    QPalette pal;
    pal.setColor(QPalette::WindowText,Qt::white);
    ui->EditValue->setPalette(pal);         //设置字体颜色

    QVBoxLayout* layouts = new QVBoxLayout();
    layouts->addWidget(state, 0);
    layouts->setContentsMargins(0,0,0,0);    //边距
    layouts->setSpacing(0);                  //部件之间的间距
    ui->stateBox->setLayout(layouts);

    QVBoxLayout* layouts2 = new QVBoxLayout();
    layouts2->addWidget(PaintArea, 0);
    layouts2->setContentsMargins(0,0,0,0);    //边距
    layouts2->setSpacing(0);                  //部件之间的间距
    ui->paintBox->setLayout(layouts2);

    //    QHBoxLayout* layout2 = new QHBoxLayout();
    //    layout2->addWidget(ui->frame);
    //    layout2->addLayout(layouts);
    //    layout2->setContentsMargins(0,0,0,0);    //边距
    //    layout2->setSpacing(0);
    //    ui->tab->setLayout(layout2);

    SetRobotTableProperty();
    SetFly();
    //创建发送线程
    s_thread = new sendthread();    //创建类对象
    _sendThread = new QThread();   //创建线程对象
    s_thread->moveToThread(_sendThread);
    connect(this, &MainWindow::sigsenddata, s_thread, &sendthread::senddata);
    _sendThread->start();

    //创建接收线程
    listmodel = new QStringListModel();
    r_thread = new mythread();    //创建类对象
    _recvThread = new QThread();   //创建线程对象
    r_thread->moveToThread(_recvThread);
    _recvThread->start();
    connect(r_thread, &mythread::display, this, &MainWindow::Strprinft);

    //connect(this, SIGNAL(sigsenddata(int)), s_thread, SLOT(senddata(int)));
    connect(s_thread, &sendthread::CriticalNofly, this, [=]()
    {
        QMessageBox::critical(this, u8"错误", u8"未找到该飞机");
    });
    connect(r_thread, &mythread::CriticalSuc, this, [=]()
    {
        ui->textEdit->append(u8"发送成功");
    });
    connect(r_thread, &mythread::CriticalEro, this, [=]()
    {
        ui->textEdit->append(tr("发送失败"));
    });

    //创建udp线程
    u_thread = new udpstates();
    _udpThread = new QThread();
    u_thread->moveToThread(_udpThread);
    _udpThread->start();
    connect(this, &MainWindow::ipdata, u_thread, &udpstates::startDateTime);
    connect(u_thread, &udpstates::result, this, &MainWindow::DUDPResult);

    connect(v_thread, &videothread::VideoData, this, &MainWindow::slotGetOneFrame);
    //connect(v_thread, SIGNAL(VideoData(QImage)), this, SLOT(slotGetOneFrame(QImage)));

    ui->PTZSlider->setMinimum(-3000);
    ui->PTZSlider->setMaximum(3000);
    ui->PTZSlider->setSingleStep(10); // 步长

    connect(r_thread, &mythread::sendArrys, this, [=](QTcpSocket* temp)
    {
        ui->textEdit->append("NewClient:" + temp->peerAddress().toString());                //在文本框中显示新链接进来的客户端的IP地址
        //qint16 port = temp->peerPort();
        ui->textEdit->append("A Client connect!");
        ui->clientnum->setText(QString::number(client_num, 10));
    });

    //设置刷新列表定时器
    m_Timer = new QTimer(this);
    connect(m_Timer, &QTimer::timeout, this, &MainWindow::slotFreshPlaneInfo);
    m_Timer->start(1200);

    //connect(this, &MainWindow::pendis, r_thread, &mythread::ppty);
    //emit pendis();

    connect(s_thread, &sendthread::writeDatas, r_thread, &mythread::sendData, Qt::BlockingQueuedConnection);
    connect(ui->BtnStart, &QPushButton::clicked, this, &MainWindow::FlyStart);

    //创建udp数据发送线程
    h_thread = new udphandle();    //创建类对象
    _udpHandle = new QThread();   //创建线程对象
    h_thread->moveToThread(_udpHandle);
    //发送完信号后发送者所在线程会阻塞，直到槽函数运行完。接收者和发送者绝对不能在一个线程，否则程序会死锁
    connect(u_thread, &udpstates::jsondata, h_thread, &udphandle::handledata, Qt::BlockingQueuedConnection);
    connect(h_thread, &udphandle::sigjsondata, s_thread, &sendthread::senddata, Qt::DirectConnection);
    connect(h_thread, &udphandle::landingid, this, &MainWindow::DLanding4);
    _udpHandle->start();


}

MainWindow::~MainWindow()
{
    delete m_Timer;

    delete ui;
}


void MainWindow::Strprinft()
{
    //qDebug()<< "display data";
    listmodel->removeRows(0, listmodel->rowCount());
    QStringList bufflist;
    QMap<int, QStringList>::iterator it;
    if(mapTimeInfo.isEmpty())
        return;
    for(it = mapRobotInfo.begin(); it!= mapRobotInfo.end(); it++)
    {
        bufflist.append(it.value());
    }

    listmodel->setStringList(bufflist); //为模型设置list，会导入list的内容
    state->ui->listView->setModel(listmodel);
}


void MainWindow::clean()
{
    ui->textEdit->clear();
}



void MainWindow::slotGetOneFrame()
{
    //qDebug() << "player";
    if(m_kPlayState == RPS_PAUSE)
    {
        return;
    }
    m_Image = img;
    update(); //调用update将执行paintEvent函数
}

/*
void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    //int showWidth = this->width() - 100;
    //int showHeight = this->height() - 50;

    painter.setBrush(Qt::white);
    painter.drawRect(0, 0, this->width(), this->height()); //先画成白色

    if (m_Image.size().width() <= 0)
    {
        return;
    }

    //将图像按比例缩放
    QImage imgs = m_Image.scaled(QSize(ui->label->width() ,  ui->label->height() -100));
    imgs = imgs.mirrored(m_bHFlip, m_bVFlip);

    int x = this->width() - imgs.width();
    int y = this->height() - imgs.height();
    x /= 2;
    y /= 2;

    QPixmap pixmap = QPixmap::fromImage(imgs);
    ui->label->setPixmap(pixmap);
    painter.drawImage(QPoint(x-40,y+20), imgs); //画出图像
}
*/

//https://www.apple.com/105/media/us/iphone-x/2017/01df5b43-28e4-4848-bf20-490c34a926a7/films/feature/iphone-x-feature-tpl-cc-us-20170912_1920x1080h.mp4
void MainWindow::DToggleds(bool checked)
{
    qDebug("ffmpeg versioin is: %u", avcodec_version());
    qDebug()<< checked;
    if (checked) //第一次按下为启动，后续则为继续
    {
        if(m_kPlayState == RPS_IDLE)
        {
            ui->lineEditUrl->setEnabled(false);
            m_strUrl = ui->lineEditUrl->text();
            if(m_strUrl.isEmpty())
                return;
            v_thread->startPlay(m_strUrl);
            qDebug(u8"rtsp网络连接中...");

        }
        m_kPlayState = RPS_RUNNING;
        ui->Btntoggled->setText(u8"暂停");
    }
    else
    {
        m_kPlayState = RPS_PAUSE;
        ui->Btntoggled->setText(u8"播放");
    }
}

//设置无人机列表的基本初始属性
void MainWindow::SetRobotTableProperty()
{
    m_nTableRowCount = 6;
    m_TableColumeCount = 6;
    int     m_nRobotMax;
    m_nRobotMax = m_nTableRowCount * m_TableColumeCount;
    ui->tableRobot->setRowCount(m_nTableRowCount);
    ui->tableRobot->setColumnCount(m_TableColumeCount);
    ui->tableRobot->setShowGrid(false);

    ui->tableRobot->horizontalHeader()->setDefaultSectionSize(105);
    ui->tableRobot->verticalHeader()->setDefaultSectionSize(92);
    ui->tableRobot->setStyleSheet("background-color:rgba(0,0,0,0)");  //设置背景透明
    ui->tableRobot->setFrameShape(QFrame::StyledPanel);                   //设置无边框

    //设置数据到表格   background-position:center
    int nRobotId = 0;
    for(int i = 0; i < m_nTableRowCount; i++)
    {
        for(int j = 0; j < m_TableColumeCount; j++)
        {
            ui->tableRobot->setItem(i, j, new QTableWidgetItem(""));
            QWidget *widget = new QWidget;
            QLabel  *editId = new QLabel;
            QString strID = QString::number(nRobotId,10);
            editId->setText(strID);
            editId->setAlignment(Qt::AlignCenter);
            editId->setStyleSheet("QLabel{color:black;font:16pt;background-position:right;}");

            //            QLabel  *editPower = new QLabel;
            //            editPower->setText("\nTest");
            //            editPower->setAlignment(Qt::AlignCenter);
            //            editPower->setStyleSheet("QLabel{color:green;font:10pt}");

            QGridLayout *markLayout = new QGridLayout(widget); //布局新加的控件
            //markLayout->addWidget(editPower);
            markLayout->addWidget(editId);
            markLayout->setContentsMargins(1,10,1,10);          //这里就可以设置单元格cell中的四个方向的间隔
            markLayout->setVerticalSpacing(0);
            widget->setLayout(markLayout);
            ui->tableRobot->setCellWidget(i, j, widget);

            // 设置点击的单元格的边框为灰色虚线边框
            ui->tableRobot->setStyleSheet("QTableWidget::item:focus {border-image: url(:/qss/fly_on.png);border:1px solid #1C1C1C;selection-color:rgb(241, 70, 62);}");
            ui->tableRobot->item(i, j)->setBackground(QBrush(QPixmap(":/qss/fly.png")));
            //设置单击单元格不响应
            ui->tableRobot->item(i, j)->setFlags(Qt::ItemIsEnabled);
            nRobotId++;
        }
    }
    //ui->tableRobot->setStyleSheet("QTableWidget::item:focus {border:1px solid #1C1C1C;}");
    //ui->tableRobot->setStyleSheet("QTableWidget::item:focus { background-color:#f5f5f5; border:none }");
}


//每隔一秒刷新无人机即时的信息
void MainWindow::slotFreshPlaneInfo()
{
    if(rowcolumn < 0)
        return;
    QPalette redPalette;
    QPalette greenPalette;
    QPalette whitePalette;
    redPalette.setColor(QPalette::Text,Qt::red);
    greenPalette.setColor(QPalette::Text, Qt::green);
    whitePalette.setColor(QPalette::Text, Qt::white);
    hgprotocol_drone_stats_t temp;
    QTcpSocket *m_tcp;
    if(trueRobotInfo.contains(rowcolumn))
    {
        temp = trueRobotInfo[rowcolumn];
    }
    else
    {
        return;
    }

    if(mapTcpsocket.contains(rowcolumn))
    {
        m_tcp = mapTcpsocket[rowcolumn];
    }
    else
    {
        QMessageBox::warning(this, u8"警告！", u8"未发现该飞机ip", QMessageBox::Yes);
        return;
    }

    ui->EditValue->setText(QString::number(temp.battery) + "%");    //电池电量设置
    ui->horizontalSlider->setValue(temp.battery);

    ui->editHorizontal->setPalette(greenPalette);       //水平距离
    int xy = sqrt((temp.x * temp.x) + (temp.y * temp.y));
    ui->editHorizontal->setText(QString::number(xy));
    ui->editVertical->setPalette(greenPalette);         //垂直距离
    ui->editVertical->setText(QString::number(temp.z));
    ui->editYAW->setPalette(greenPalette);              //yaw
    ui->editYAW->setText(QString::number(temp.yaw));

    ui->editIP->setPalette(greenPalette);              //ip地址
    QString strSatellite = m_tcp->peerAddress().toString();
    ui->editIP->setText(strSatellite);


    //定位状态
    ui->editState->setPalette(whitePalette);
    if((temp.sensor_status & MAV_SYS_SENSOR_RTK))
    {
        if(((temp.rtk_stats & 0xC0) >> 6) == 0x00)
        {
            ui->editState->setText(tr(u8"无定位"));
            ui->editState->setPalette(redPalette);
        }
        else if(((temp.rtk_stats & 0xC0) >> 6) == 0x02)
        {
            ui->editState->setText(u8"Float");
            ui->editState->setPalette(redPalette);
        }
        else if(((temp.rtk_stats & 0xC0) >> 6) == 0x03)
        {
            ui->editState->setText(u8"Fix");
            ui->editState->setPalette(greenPalette);
        }
        else
        {
            ui->editState->setText(tr(u8"3D定位"));
            ui->editState->setPalette(redPalette);
        }
    }
    else
    {
        ui->editState->setText(tr(u8"未检测到"));
        ui->editState->setPalette(redPalette);
    }


    ui->editBarometer->setPalette(whitePalette);        //气压计
    if((temp.sensor_status & MAV_SYS_SENSOR_BARO))
    {
        ui->editBarometer->setText("Ok");
        ui->editBarometer->setPalette(greenPalette);
    }
    else
    {
        ui->editBarometer->setText("NO");
        ui->editBarometer->setPalette(redPalette);
    }

    ui->editIMU1->setPalette(whitePalette);              //IMU1
    if((temp.sensor_status & MAV_SYS_SENSOR_IMU1))
    {
        if(((temp.imu_status[0] & MAV_IMU_STATUS_ABNORMAL)))
        {
            ui->editIMU1->setText(tr(u8"数据异常"));
            ui->editIMU1->setPalette(redPalette);
        }
        else if(((temp.imu_status[0] & MAV_IMU_STATUS_GYRO_CAL)) &&
                ((temp.imu_status[0] & MAV_IMU_STATUS_ACCEL_CAL)) &&
                ((temp.imu_status[0] & MAV_IMU_STATUS_HEALTHY)))
        {
            ui->editIMU1->setText("OK");
            ui->editIMU1->setPalette(greenPalette);
        }
        else if(((temp.imu_status[0] & MAV_IMU_STATUS_GYRO_CAL) != MAV_IMU_STATUS_GYRO_CAL) ||
                ((temp.imu_status[0] & MAV_IMU_STATUS_ACCEL_CAL)!= MAV_IMU_STATUS_ACCEL_CAL))
        {
            ui->editIMU1->setText(tr(u8"未校准"));
            ui->editIMU1->setPalette(redPalette);
        }
        else if(((temp.imu_status[0] & MAV_IMU_STATUS_HEALTHY) != MAV_IMU_STATUS_HEALTHY))
        {

            ui->editIMU1->setText(tr(u8"不健康"));
            ui->editIMU1->setPalette(redPalette);
        }
    }
    else
    {
        ui->editIMU1->setText(tr(u8"未检测到"));
        ui->editIMU1->setPalette(redPalette);
    }

    ui->editIMU2->setPalette(whitePalette);              //IMU2
    if((temp.sensor_status & MAV_SYS_SENSOR_IMU2))
    {
        if(((temp.imu_status[1] & MAV_IMU_STATUS_ABNORMAL)))
        {
            ui->editIMU2->setText(tr(u8"数据异常"));
            ui->editIMU2->setPalette(redPalette);
        }
        else if(((temp.imu_status[1] & MAV_IMU_STATUS_GYRO_CAL)) &&
                ((temp.imu_status[1] & MAV_IMU_STATUS_ACCEL_CAL)) &&
                ((temp.imu_status[1] & MAV_IMU_STATUS_HEALTHY)))
        {
            ui->editIMU2->setText("OK");
            ui->editIMU2->setPalette(greenPalette);
        }
        else if(((temp.imu_status[1] & MAV_IMU_STATUS_GYRO_CAL) != MAV_IMU_STATUS_GYRO_CAL) ||
                ((temp.imu_status[1] & MAV_IMU_STATUS_ACCEL_CAL)!= MAV_IMU_STATUS_ACCEL_CAL))
        {
            ui->editIMU2->setText(tr(u8"未校准"));
            ui->editIMU2->setPalette(redPalette);
        }
        else if(((temp.imu_status[1] & MAV_IMU_STATUS_HEALTHY) != MAV_IMU_STATUS_HEALTHY))
        {

            ui->editIMU2->setText(tr(u8"不健康"));
            ui->editIMU2->setPalette(redPalette);
        }
    }
    else
    {
        ui->editIMU2->setText(tr(u8"未检测到"));
        ui->editIMU2->setPalette(redPalette);
    }


    ui->editIMagnetometer1->setPalette(whitePalette);    //磁力计1
    if((temp.sensor_status & MAV_SYS_SENSOR_MAG))
    {
        if(((temp.imu_status[0] & MAV_MAG_STATUS_ABNORMAL)))
        {
            ui->editIMagnetometer1->setText(tr(u8"总线异常"));
            ui->editIMagnetometer1->setPalette(redPalette);
        }
        else if(((temp.mag_status[0] & MAV_MAG_STATUS_CAL)) &&
                ((temp.mag_status[0] & MAV_MAG_STATUS_HEALTHY)))
        {
            ui->editIMagnetometer1->setText("OK");
            ui->editIMagnetometer1->setPalette(greenPalette);
        }
        else if(((temp.mag_status[0] & MAV_MAG_STATUS_CAL) != MAV_MAG_STATUS_CAL))
        {
            ui->editIMagnetometer1->setText(tr(u8"未校准"));
            ui->editIMagnetometer1->setPalette(redPalette);
        }
        else if(((temp.mag_status[0] & MAV_MAG_STATUS_HEALTHY) != MAV_MAG_STATUS_HEALTHY))
        {

            ui->editIMagnetometer1->setText(tr(u8"不健康"));
            ui->editIMagnetometer1->setPalette(redPalette);
        }
    }
    else
    {
        ui->editIMagnetometer1->setText(tr(u8"未检测到"));
        ui->editIMagnetometer1->setPalette(redPalette);
    }

    ui->editIMagnetometer2->setPalette(whitePalette);    //磁力计2
    if((temp.sensor_status & MAV_SYS_SENSOR_MAG2))
    {
        if(((temp.imu_status[1] & MAV_MAG_STATUS_ABNORMAL)))
        {
            ui->editIMagnetometer2->setText(tr(u8"总线异常"));
            ui->editIMagnetometer2->setPalette(redPalette);
        }
        else if(((temp.mag_status[1] & MAV_MAG_STATUS_CAL)) &&
                ((temp.mag_status[1] & MAV_MAG_STATUS_HEALTHY)))
        {
            ui->editIMagnetometer2->setText("OK");
            ui->editIMagnetometer2->setPalette(greenPalette);
        }
        else if(((temp.mag_status[1] & MAV_MAG_STATUS_CAL) != MAV_MAG_STATUS_CAL))
        {
            ui->editIMagnetometer2->setText(tr(u8"未校准"));
            ui->editIMagnetometer2->setPalette(redPalette);
        }
        else if(((temp.mag_status[1] & MAV_MAG_STATUS_HEALTHY) != MAV_MAG_STATUS_HEALTHY))
        {

            ui->editIMagnetometer2->setText(tr(u8"不健康"));
            ui->editIMagnetometer2->setPalette(redPalette);
        }
    }
    else
    {
        ui->editIMagnetometer2->setText(tr(u8"未检测到"));
        ui->editIMagnetometer2->setPalette(redPalette);
    }
}


//点击表格无人机状态信息
void MainWindow::slotDisplayPlaneInfo(int row, int column)
{
    // qDebug() << u8"所点击的单元格的row=" << row << ",col=" << column;
    rowbuff = row;
    colbuff = column;
    hgprotocol_drone_stats_t temp;
    rowcolumn = row*6+column;   //飞机ID
    ui->Editnum->setText(QString::number(row*6+column));
    //QTableWidgetItem *selectedItem =  ui->tableRobot->item(row, column);
    qDebug() <<u8"所选飞机：=" << row*6+column;
    if(trueRobotInfo.contains(row*6+column))
    {
        temp = trueRobotInfo[row*6+column];
    }
    else
    {
        QMessageBox::warning(this, u8"警告！", u8"未发现该飞机心跳数据", QMessageBox::Yes);
        return;
    }
}


//设置比赛无人机列表
void MainWindow::SetFly()
{
    int m_nTableRowCount = 3;
    int m_TableColumeCount = 1;
    int     m_nRobotMax;
    m_nRobotMax = m_nTableRowCount * m_TableColumeCount;
    ui->flyWidget->setRowCount(m_nTableRowCount);
    ui->flyWidget->setColumnCount(m_TableColumeCount);
    ui->flyWidget->setShowGrid(false);

    ui->flyWidget->horizontalHeader()->setDefaultSectionSize(50);
    ui->flyWidget->verticalHeader()->setDefaultSectionSize(50);
    ui->flyWidget->setStyleSheet("background-color:rgba(0,0,0,0)");  //设置背景透明
    ui->flyWidget->setFrameShape(QFrame::StyledPanel);                   //设置无边框

    //设置数据到表格   background-position:center
    int nRobotId = 1;
    for(int i = 0; i < m_nTableRowCount; i++)
    {
        for(int j = 0; j < m_TableColumeCount; j++)
        {
            ui->flyWidget->setItem(i, j, new QTableWidgetItem(""));
            QWidget *widget = new QWidget;
            QLabel  *editId = new QLabel;
            QString strID = QString::number(nRobotId,10) ;
            editId->setText(strID);
            editId->setAlignment(Qt::AlignCenter);
            editId->setStyleSheet("QLabel{color:black;font:10pt;background-position:right;}");

            QGridLayout *markLayout = new QGridLayout(widget); //布局新加的控件
            //markLayout->addWidget(editPower);
            markLayout->addWidget(editId);
            markLayout->setContentsMargins(1,10,1,10);          //这里就可以设置单元格cell中的四个方向的间隔
            markLayout->setVerticalSpacing(0);
            widget->setLayout(markLayout);
            ui->flyWidget->setCellWidget(i, j, widget);

            // 设置点击的单元格的边框为灰色虚线边框
            ui->flyWidget->setStyleSheet("QTableWidget::item:focus {border-image: url(:/qss/fly_on.png);border:1px solid #1C1C1C;selection-color:rgb(241, 70, 62);}");
            ui->flyWidget->item(i, j)->setBackground(QBrush(QPixmap(":/qss/fly_ui.png")));
            //设置单击单元格不响应
            //ui->flyWidget->item(i, j)->setFlags(Qt::ItemIsEnabled);
            nRobotId++;
        }
    }
}


void MainWindow::slotflyInfo(int row, int column)
{
    FLY_SET_DATA setdata;
    rowbuff = row;
    colbuff = column;
    rowcolumn = row+column+1;   //飞机ID
    ui->Editnum2->setText(QString::number(rowcolumn));
    //ui->Editnum->setText(QString::number(row*6+column));
    //QTableWidgetItem *selectedItem =  ui->tableRobot->item(row, column);
    qDebug() <<u8"所选飞机：=" << rowcolumn;
    setdata = mapSetFlyData[rowcolumn] ;
    //将map数据展示
    QMap<int, FLY_SET_DATA>::iterator it;
    it = mapSetFlyData.find(rowcolumn);
    if (it != mapSetFlyData.end())    //找到数据
    {
        if(it.value().direction == QString("0"))
        {
           setdata.direction = QString("前");
        }
        else if(it.value().direction == QString("1"))
        {
            setdata.direction = QString("右前");
        }
        else if(it.value().direction == QString("2"))
        {
            setdata.direction = QString("右");
        }
        else if(it.value().direction == QString("3"))
        {
            setdata.direction = QString("右后");
        }
        else if(it.value().direction == QString("4"))
        {
            setdata.direction = QString("后");
        }
        else if(it.value().direction == QString("5"))
        {
            setdata.direction = QString("左后");
        }
        else if(it.value().direction == QString("6"))
        {
            setdata.direction = QString("左");
        }
        else if(it.value().direction == QString("7"))
        {
            setdata.direction = QString("左前");
        }
        else if(it.value().direction == QString("8"))
        {
            setdata.direction = QString("中");
        }

        if(it.value().covering == QString("1"))
        {
            setdata.covering = QString("是");
        }
        else if(it.value().covering == QString("0"))
        {
            setdata.covering = QString("否");
        }

        ui->EditRadius->setText(setdata.radius);
        ui->EditCar->setCurrentText(setdata.carnum);
        ui->BoxDirection->setCurrentText(setdata.direction);
        ui->EditSpeed->setText(setdata.speed);
        ui->BoxCovering->setCurrentText(setdata.covering);
        ui->EditHeight->setText(setdata.height);
    }

}

/*-----------------------------隐藏函数--------------------------------------*/
void MainWindow::DRoot()
{
    QString xLat =  ui->XLat->text();
    QString yLat =  ui->YLat->text();
    QString zLat =  ui->ZLat->text();

    if(xLat == yLat && xLat == zLat && yLat == zLat)
    {
        //ui->comlabel->setText(u8"串口连接");
        ui->openCOM->setEnabled(true);
        //ui->FlyUnlock->setEnabled(true);
        //ui->Flylock->setEnabled(true);
        ui->Takeoff->setEnabled(true);
        ui->Landing->setEnabled(true);
        ui->Flyreturn->setEnabled(true);
        ui->Setid->setEnabled(true);
        ui->Setangle->setEnabled(true);
        ui->Clogo->setEnabled(true);
        ui->Cougo->setEnabled(true);
    }
}
/*--------------------------------------------------------------------------*/


/*-----------------------------发送函数--------------------------------------*/
int MainWindow::senddata(const uint8_t *data, int length, int id)
{
    if((length <= 0) || (data == NULL))
    {
        ui->textEdit->append(QString("generate cmd frame input agrs error"));
        qDebug() <<"generate cmd frame input agrs error";
        return 1;
    }

    QByteArray byteBuf;
    byteBuf.clear();
    byteBuf.resize(length);
    memcpy(byteBuf.data(), data, length);
    //mapSenddata.insert(id, byteBuf);
    mutexlock.lock();
    mapSenddata[id] = byteBuf;
    mutexlock.unlock();

    QString str;
    for(int i = 0; i < length; i++)
    {
        str += QString("%1").arg((unsigned char)byteBuf.at(i), 2, 16, QLatin1Char('0') ) + ' ';
    }
    qDebug() << length;
    qDebug() << str;
    //m_socket->write(byteBuf);
    emit sigsenddata(id);
    return 0;
}
/*--------------------------------------------------------------------------*/


/*----------------------------初始化-----------------------------------------*/
void MainWindow::DInitUDP()
{
    QString pcip = ui->ip->text();
    emit ipdata(pcip);
}
/*--------------------------------------------------------------------------*/


/*---------------------------初始化结果----------------------------------------*/
void MainWindow::DUDPResult(int ret)
{
    if(ret == 0)
    {
        ui->textEdit->append(u8"UDP发送失败");
        ui->textEdit->append(u8"串口打开失败");
    }
    else
    {
        ui->comlabel->setText(u8"网络连接");
        //ui->textEdit->append(QString("串口(%1)打开成功").arg(strComboBoxContent));
        ui->textEdit->append(u8"网络连接成功");
        ui->openCOM->setEnabled(true);
        ui->FlyUnlock->setEnabled(true);
        ui->Flylock->setEnabled(true);
        ui->Takeoff->setEnabled(true);
        ui->Landing->setEnabled(true);
        ui->Flyreturn->setEnabled(true);
        ui->Setid->setEnabled(true);
        ui->Setangle->setEnabled(true);
        ui->Clogo->setEnabled(true);
        ui->Cougo->setEnabled(true);
        ui->FlyStop->setEnabled(true);
        ui->Btntoggled->setEnabled(true);
    }
}
/*--------------------------------------------------------------------------*/


int my_strlen(uint16_t* arr)
{
    int count = 0;
    while (*arr) {
        count++;
        arr++;
    }
    return count;
}


/*-----------------------------设置ID----------------------------------------*/
void MainWindow::DSetID()
{
    QString start =  ui->flystart->text();
    QString end   = ui->flyend->text();
    if(start.isEmpty())
    {
        ui->textEdit->append(u8"注意飞机ID位空");
        return;
    }
    QString str =  ui->flyid->text();

    uint8_t sbuf[512] = {0};
    hgprotocol_message_t  msg;
    hgprotocol_lcommand_t flydata;
    memset(&flydata, 0, sizeof(hgprotocol_lcommand_t));
    flydata.cmd = MAV_CMD_SETID;
    flydata.start_id = start.toUShort();
    flydata.end_id = start.toUShort();
    flydata.param[0] = str.toInt();  //设置飞机ID
    hgprotocol_lcommand_encode(0, 0, &msg, &flydata);
    int size = hgprotocol_msg_to_send_buffer(sbuf, &msg);

    senddata(sbuf, size, flydata.start_id);
    ui->textEdit->append( QString(u8"飞机:%1 设置ID已发送").arg(flydata.start_id));
}
/*--------------------------------------------------------------------------*/


/*------------------------------解锁-----------------------------------------*/
void MainWindow::DFlyunLock()
{
    QString start =  ui->flystart->text();
    QString end   = ui->flyend->text();
    if(start.isEmpty())
        ui->textEdit->append(u8"注意飞机ID位空");

    uint8_t sbuf[512] = {0};
    hgprotocol_message_t  msg;
    hgprotocol_lcommand_t flydata;
    memset(&flydata, 0, sizeof(hgprotocol_lcommand_t));
    flydata.cmd = MAV_CMD_UNLOCK;
    flydata.start_id = start.toUShort();
    flydata.end_id = end.toUShort();
    flydata.param[0] = 0x02;
    hgprotocol_lcommand_encode(0, 0, &msg, &flydata);
    int size = hgprotocol_msg_to_send_buffer(sbuf, &msg);

    int mun = end.toInt() - start.toInt() + 1;
    for(int i = 0; i < mun; i++)
    {
        flydata.start_id = start.toUShort() + i;
        //flydata.end_id = flydata.start_id;
        senddata(sbuf, size, flydata.start_id);
        ui->textEdit->append( QString(u8"飞机:%1 解锁已发送").arg(flydata.start_id));
    }
}
/*--------------------------------------------------------------------------*/


/*------------------------------上锁-----------------------------------------*/
void MainWindow::DFlyLock()
{
    QString start =  ui->flystart->text();
    QString end   = ui->flyend->text();
    if(start.isEmpty())
        ui->textEdit->append(u8"注意飞机ID位空");

    uint8_t sbuf[512] = {0};
    hgprotocol_message_t  msg;
    hgprotocol_lcommand_t flydata;
    memset(&flydata, 0, sizeof(hgprotocol_lcommand_t));
    flydata.cmd = MAV_CMD_LOCK;
    flydata.start_id = start.toUShort();
    flydata.end_id = end.toUShort();
    flydata.param[0] = 0x01;
    hgprotocol_lcommand_encode(0, 0, &msg, &flydata);
    int size = hgprotocol_msg_to_send_buffer(sbuf, &msg);

    int mun = end.toInt() - start.toInt() + 1;
    for(int i = 0; i < mun; i++)
    {
        flydata.start_id = start.toUShort() + i;
        //flydata.end_id = flydata.start_id;
        senddata(sbuf, size, flydata.start_id);
        ui->textEdit->append( QString(u8"飞机:%1 上锁已发送").arg(flydata.start_id));
    }
}
/*--------------------------------------------------------------------------*/


/*------------------------------起飞-----------------------------------------*/
void MainWindow::DTakeOff()
{
    QString start =  ui->flystart->text();
    QString end   = ui->flyend->text();
    if(start.isEmpty())
        ui->textEdit->append(u8"注意飞机ID位空");

    uint8_t sbuf[512] = {0};
    hgprotocol_message_t  msg;
    hgprotocol_lcommand_t flydata;
    memset(&flydata, 0, sizeof(hgprotocol_lcommand_t));
    flydata.cmd = MAV_CMD_TAKEOFF;
    flydata.start_id = start.toUShort();
    flydata.end_id = end.toUShort();
    flydata.param[0] = 1000;
    flydata.param[1] = 24;
    flydata.param[2] = 0x8000|0x4000;
    hgprotocol_lcommand_encode(0, 0, &msg, &flydata);
    int size = hgprotocol_msg_to_send_buffer(sbuf, &msg);

    int mun = end.toInt() - start.toInt() + 1;
    for(int i = 0; i < mun; i++)
    {
        flydata.start_id = start.toUShort() + i;
        //flydata.end_id = flydata.start_id;
        senddata(sbuf, size, flydata.start_id);
        ui->textEdit->append( QString(u8"飞机:%1 起飞已发送").arg(flydata.start_id));
    }
}
/*--------------------------------------------------------------------------*/


/*------------------------------降落-----------------------------------------*/
void MainWindow::DLanding()
{
    QString start =  ui->flystart->text();
    QString end   = ui->flyend->text();
    if(start.isEmpty())
        ui->textEdit->append(u8"注意飞机ID位空");
    //    uint16_t data[5];
    //    payload_net_data *p = (payload_net_data *)buff;
    //    p->data[0] =  0x2000;

    uint8_t sbuf[512] = {0};
    hgprotocol_message_t  msg;
    hgprotocol_lcommand_t flydata;
    memset(&flydata, 0, sizeof(hgprotocol_lcommand_t));
    flydata.cmd = MAV_CMD_LAND;
    flydata.start_id = start.toUShort();
    flydata.end_id = end.toUShort();
    flydata.param[0] = 0x2000;
    hgprotocol_lcommand_encode(0, 0, &msg, &flydata);
    int size = hgprotocol_msg_to_send_buffer(sbuf, &msg);

    int mun = end.toInt() - start.toInt() + 1;
    for(int i = 0; i < mun; i++)
    {
        flydata.start_id = start.toUShort() + i;
        //flydata.end_id = flydata.start_id;
        senddata(sbuf, size, flydata.start_id);
        ui->textEdit->append( QString(u8"飞机:%1 降落已发送").arg(flydata.start_id));
    }
}
/*--------------------------------------------------------------------------*/


/*------------------------------返航-----------------------------------------*/
void MainWindow::DFlyReturn()
{
    QString heightstr =  ui->height->text();
    QString start =  ui->flystart->text();
    QString end   = ui->flyend->text();
    if(start.isEmpty())
        ui->textEdit->append(u8"注意飞机ID位空");
    if(heightstr.isEmpty())
        ui->textEdit->append(u8"飞机高度为空");

    uint8_t sbuf[512] = {0};
    hgprotocol_message_t  msg;
    hgprotocol_lcommand_t flydata;
    memset(&flydata, 0, sizeof(hgprotocol_lcommand_t));
    flydata.cmd = MAV_CMD_DRONE_BASE;
    flydata.start_id = start.toUShort();
    flydata.end_id = end.toUShort();
    flydata.param[0] = heightstr.toInt();
    hgprotocol_lcommand_encode(0, 0, &msg, &flydata);
    int size = hgprotocol_msg_to_send_buffer(sbuf, &msg);

    int mun = end.toInt() - start.toInt() + 1;
    for(int i = 0; i < mun; i++)
    {
        flydata.start_id = start.toUShort() + i;
        //flydata.end_id = flydata.start_id;
        senddata(sbuf, size, flydata.start_id);
        ui->textEdit->append( QString(u8"飞机:%1 返航已发送").arg(flydata.start_id));
    }
}
/*--------------------------------------------------------------------------*/


/*----------------------------航点控制---------------------------------------*/
void MainWindow::DSetAngle()
{
    QString xLat =  ui->XLat->text();
    QString yLat =  ui->YLat->text();
    QString zLat =  ui->ZLat->text();
    QString Zyaw =  ui->Zyaw->text();
    QString start =  ui->flystart->text();
    QString end   = ui->flyend->text();
    QString speed = ui->speed->text();
    if(start.isEmpty())
        ui->textEdit->append(u8"注意飞机ID位空");

    uint8_t sbuf[512] = {0};
    hgprotocol_message_t  msg;
    hgprotocol_lcommand_t flydata;
    memset(&flydata, 0, sizeof(hgprotocol_lcommand_t));
    flydata.cmd = MAV_CMD_GO;
    flydata.start_id = start.toUShort();
    flydata.end_id = end.toUShort();
    flydata.param[0] = xLat.toInt();           // -500 - 500
    flydata.param[1] = yLat.toInt();           // -500 - 500
    flydata.param[2] = zLat.toInt();           // -500 - 500
    flydata.param[3] = speed.toInt();           //10-100(cm/s)
    flydata.param[4] = Zyaw.toInt() * 10;
    hgprotocol_lcommand_encode(0, 0, &msg, &flydata);
    int size = hgprotocol_msg_to_send_buffer(sbuf, &msg);

    int mun = end.toInt() - start.toInt() + 1;
    for(int i = 0; i < mun; i++)
    {
        flydata.start_id = start.toUShort() + i;
        //flydata.end_id = flydata.start_id;
        senddata(sbuf, size, flydata.start_id);
        ui->textEdit->append( QString(u8"飞机:%1 航点控制已发送").arg(flydata.start_id));
    }
}
/*--------------------------------------------------------------------------*/


/*----------------------------顺时针旋转--------------------------------------*/
void MainWindow::DCloGo()
{
    QString anglestr =  ui->angleclo->text();
    QString start =  ui->flystart->text();
    QString end   = ui->flyend->text();
    if(start.isEmpty())
        ui->textEdit->append(u8"注意飞机ID位空");

    uint8_t sbuf[512] = {0};
    hgprotocol_message_t  msg;
    hgprotocol_lcommand_t flydata;
    memset(&flydata, 0, sizeof(hgprotocol_lcommand_t));
    flydata.cmd = MAV_CMD_CW;
    flydata.start_id = start.toUShort();
    flydata.end_id = end.toUShort();
    flydata.param[0] = anglestr.toInt() * 10;
    hgprotocol_lcommand_encode(0, 0, &msg, &flydata);
    int size = hgprotocol_msg_to_send_buffer(sbuf, &msg);

    int mun = end.toInt() - start.toInt() + 1;
    for(int i = 0; i < mun; i++)
    {
        flydata.start_id = start.toUShort() + i;
        //flydata.end_id = flydata.start_id;
        senddata(sbuf, size, flydata.start_id);
        ui->textEdit->append( QString(u8"飞机:%1 顺时针旋转已发送").arg(flydata.start_id));
    }
}
/*--------------------------------------------------------------------------*/


/*----------------------------逆时针旋转--------------------------------------*/
void MainWindow::DCouGo()

{
    QString anglestr =  ui->anglecou->text();
    QString start =  ui->flystart->text();
    QString end   = ui->flyend->text();
    if(start.isEmpty())
        ui->textEdit->append(u8"注意飞机ID位空");

    uint8_t sbuf[512] = {0};
    hgprotocol_message_t  msg;
    hgprotocol_lcommand_t flydata;
    memset(&flydata, 0, sizeof(hgprotocol_lcommand_t));
    flydata.cmd = MAV_CMD_CCW;
    flydata.start_id = start.toUShort();
    flydata.end_id = end.toUShort();
    flydata.param[0] = anglestr.toInt() * 10;
    hgprotocol_lcommand_encode(0, 0, &msg, &flydata);
    int size = hgprotocol_msg_to_send_buffer(sbuf, &msg);

    int mun = end.toInt() - start.toInt() + 1;
    for(int i = 0; i < mun; i++)
    {
        flydata.start_id = start.toUShort() + i;
        //flydata.end_id = flydata.start_id;
        senddata(sbuf, size, flydata.start_id);
        ui->textEdit->append( QString(u8"飞机:%1 逆时针旋转已发送").arg(flydata.start_id));
    }
}
/*--------------------------------------------------------------------------*/


/*-------------------------------悬停----------------------------------------*/
void MainWindow::DFlyStop()
{
    QString start =  ui->flystart->text();
    QString end   = ui->flyend->text();
    if(start.isEmpty())
        ui->textEdit->append(u8"注意飞机ID位空");

    uint8_t sbuf[512] = {0};
    hgprotocol_message_t  msg;
    hgprotocol_lcommand_t flydata;
    memset(&flydata, 0, sizeof(hgprotocol_lcommand_t));
    flydata.cmd = MAV_CMD_STOP;
    flydata.start_id = start.toUShort();
    flydata.end_id = end.toUShort();
    hgprotocol_lcommand_encode(0, 0, &msg, &flydata);
    int size = hgprotocol_msg_to_send_buffer(sbuf, &msg);

    int mun = end.toInt() - start.toInt() + 1;
    for(int i = 0; i < mun; i++)
    {
        flydata.start_id = start.toUShort() + i;
        //flydata.end_id = flydata.start_id;
        senddata(sbuf, size, flydata.start_id);
        ui->textEdit->append( QString(u8"飞机:%1 悬停已发送").arg(flydata.start_id));
    }
}
/*--------------------------------------------------------------------------*/


/*------------------------------降落-----------------------------------------*/
void MainWindow::DLanding2()
{
    //ui->tableRobot->setStyleSheet("background-image: url(:/qss/fly_on.png)");
    //ui->tableRobot->setStyleSheet("QTableWidget::item:focus {border-image: url(:/qss/fly_on.png);border:1px solid #1C1C1C;selection-color:rgb(241, 70, 62);}");
    //ui->tableRobot->setStyleSheet("QTableWidget {border-image: url(:/qss/fly.png);}");
    //ui->tableRobot->item(rowbuff, colbuff)->setBackground(QBrush(QPixmap(":/qss/fly_on.png")));
    //ui->tableRobot->item(rowbuff,colbuff)->setCheckState(Qt::PartiallyChecked);

    if(temporaryR != rowbuff || temporaryC != colbuff)
    {
        ui->tableRobot->item(temporaryR, temporaryC)->setBackground(QPixmap(":/qss/fly.png"));
        temporaryR = rowbuff;
        temporaryC = colbuff;
        ui->tableRobot->item(rowbuff, colbuff)->setBackground(QPixmap(":/qss/fly_on.png")) ;
    }
    else
        ui->tableRobot->item(rowbuff, colbuff)->setBackground(QPixmap(":/qss/fly_on.png")) ;
    QString start =  ui->Editnum->text();
    if(start.isEmpty())
        ui->textEdit->append(u8"注意飞机ID位空");

    uint8_t sbuf[512] = {0};
    hgprotocol_message_t  msg;
    hgprotocol_lcommand_t flydata;
    memset(&flydata, 0, sizeof(hgprotocol_lcommand_t));
    flydata.cmd = MAV_CMD_LAND;
    flydata.start_id = start.toUShort();
    flydata.end_id = start.toUShort();
    flydata.param[0] = 0x2000;
    hgprotocol_lcommand_encode(0, 0, &msg, &flydata);
    int size = hgprotocol_msg_to_send_buffer(sbuf, &msg);

    senddata(sbuf, size, flydata.start_id);
    ui->textEdit->append( QString(u8"飞机:%1 降落已发送").arg(flydata.start_id));
}
/*--------------------------------------------------------------------------*/


/*------------------------------起飞-----------------------------------------*/
void MainWindow::DTakeOff2()
{
    if(temporaryR != rowbuff || temporaryC != colbuff)
    {
        ui->tableRobot->item(temporaryR, temporaryC)->setBackground(QPixmap(":/qss/fly.png"));
        temporaryR = rowbuff;
        temporaryC = colbuff;
        ui->tableRobot->item(rowbuff, colbuff)->setBackground(QPixmap(":/qss/fly_on.png")) ;
    }
    else
        ui->tableRobot->item(rowbuff, colbuff)->setBackground(QPixmap(":/qss/fly_on.png")) ;
    QString start =  ui->Editnum->text();
    if(start.isEmpty())
        ui->textEdit->append(u8"注意飞机ID位空");

    uint8_t sbuf[512] = {0};
    hgprotocol_message_t  msg;
    hgprotocol_lcommand_t flydata;
    memset(&flydata, 0, sizeof(hgprotocol_lcommand_t));
    flydata.cmd = MAV_CMD_TAKEOFF;
    flydata.start_id = start.toUShort();
    flydata.end_id = start.toUShort();
    flydata.param[0] = 1000;
    flydata.param[1] = 24;
    flydata.param[2] = 0x8000|0x4000;
    hgprotocol_lcommand_encode(0, 0, &msg, &flydata);
    int size = hgprotocol_msg_to_send_buffer(sbuf, &msg);

    senddata(sbuf, size, flydata.start_id);
    ui->textEdit->append( QString(u8"飞机:%1 起飞已发送").arg(flydata.start_id));
}
/*--------------------------------------------------------------------------*/


/*------------------------------返航-----------------------------------------*/
void MainWindow::DFlyReturn2()
{
    if(temporaryR != rowbuff || temporaryC != colbuff)
    {
        ui->tableRobot->item(temporaryR, temporaryC)->setBackground(QPixmap(":/qss/fly.png"));
        temporaryR = rowbuff;
        temporaryC = colbuff;
        ui->tableRobot->item(rowbuff, colbuff)->setBackground(QPixmap(":/qss/fly_on.png")) ;
    }
    else
        ui->tableRobot->item(rowbuff, colbuff)->setBackground(QPixmap(":/qss/fly_on.png")) ;
    QString heightstr =  ui->height2->text();
    QString start =  ui->Editnum->text();
    if(start.isEmpty())
        ui->textEdit->append(u8"注意飞机ID位空");
    if(heightstr.isEmpty())
        ui->textEdit->append(u8"飞机高度为空");

    uint8_t sbuf[512] = {0};
    hgprotocol_message_t  msg;
    hgprotocol_lcommand_t flydata;
    memset(&flydata, 0, sizeof(hgprotocol_lcommand_t));
    flydata.cmd = MAV_CMD_DRONE_BASE;
    flydata.start_id = start.toUShort();
    flydata.end_id = start.toUShort();
    flydata.param[0] = heightstr.toInt();
    hgprotocol_lcommand_encode(0, 0, &msg, &flydata);
    int size = hgprotocol_msg_to_send_buffer(sbuf, &msg);

    senddata(sbuf, size, flydata.start_id);
    ui->textEdit->append( QString(u8"飞机:%1 返航已发送").arg(flydata.start_id));
}
/*--------------------------------------------------------------------------*/


/*------------------------------向上-----------------------------------------*/
void MainWindow::DFlyUp()
{
    if(temporaryR != rowbuff || temporaryC != colbuff)
    {
        ui->tableRobot->item(temporaryR, temporaryC)->setBackground(QPixmap(":/qss/fly.png"));
        temporaryR = rowbuff;
        temporaryC = colbuff;
        ui->tableRobot->item(rowbuff, colbuff)->setBackground(QPixmap(":/qss/fly_on.png")) ;
    }
    else
        ui->tableRobot->item(rowbuff, colbuff)->setBackground(QPixmap(":/qss/fly_on.png")) ;
    QString start =  ui->Editnum->text();
    QString distance = ui->Distance->text();
    if(start.isEmpty())
        ui->textEdit->append(u8"注意飞机ID位空");
    if(distance.isEmpty())
        ui->textEdit->append(u8"飞行距离为空");

    uint8_t sbuf[512] = {0};
    hgprotocol_message_t  msg;
    hgprotocol_lcommand_t flydata;
    memset(&flydata, 0, sizeof(hgprotocol_lcommand_t));
    flydata.cmd = MAV_CMD_UP;
    flydata.start_id = start.toUShort();
    flydata.end_id = start.toUShort();
    flydata.param[0] = distance.toInt();
    hgprotocol_lcommand_encode(0, 0, &msg, &flydata);
    int size = hgprotocol_msg_to_send_buffer(sbuf, &msg);

    senddata(sbuf, size, flydata.start_id);
    ui->textEdit->append( QString(u8"飞机:%1 向上已发送").arg(flydata.start_id));
}
/*--------------------------------------------------------------------------*/


/*------------------------------向下-----------------------------------------*/
void MainWindow::DFlyDown()
{
    if(temporaryR != rowbuff || temporaryC != colbuff)
    {
        ui->tableRobot->item(temporaryR, temporaryC)->setBackground(QPixmap(":/qss/fly.png"));
        temporaryR = rowbuff;
        temporaryC = colbuff;
        ui->tableRobot->item(rowbuff, colbuff)->setBackground(QPixmap(":/qss/fly_on.png")) ;
    }
    else
        ui->tableRobot->item(rowbuff, colbuff)->setBackground(QPixmap(":/qss/fly_on.png")) ;
    QString start =  ui->Editnum->text();
    QString distance = ui->Distance->text();
    if(start.isEmpty())
        ui->textEdit->append(u8"注意飞机ID位空");
    if(distance.isEmpty())
        ui->textEdit->append(u8"飞行距离为空");

    uint8_t sbuf[512] = {0};
    hgprotocol_message_t  msg;
    hgprotocol_lcommand_t flydata;
    memset(&flydata, 0, sizeof(hgprotocol_lcommand_t));
    flydata.cmd = MAV_CMD_DOWN;
    flydata.start_id = start.toUShort();
    flydata.end_id = start.toUShort();
    flydata.param[0] = distance.toInt();
    hgprotocol_lcommand_encode(0, 0, &msg, &flydata);
    int size = hgprotocol_msg_to_send_buffer(sbuf, &msg);

    senddata(sbuf, size, flydata.start_id);
    ui->textEdit->append( QString(u8"飞机:%1 向下已发送").arg(flydata.start_id));

}
/*--------------------------------------------------------------------------*/


/*------------------------------向后-----------------------------------------*/
void MainWindow::DFlyBack()
{
    if(temporaryR != rowbuff || temporaryC != colbuff)
    {
        ui->tableRobot->item(temporaryR, temporaryC)->setBackground(QPixmap(":/qss/fly.png"));
        temporaryR = rowbuff;
        temporaryC = colbuff;
        ui->tableRobot->item(rowbuff, colbuff)->setBackground(QPixmap(":/qss/fly_on.png")) ;
    }
    else
        ui->tableRobot->item(rowbuff, colbuff)->setBackground(QPixmap(":/qss/fly_on.png")) ;
    QString start =  ui->Editnum->text();
    QString distance = ui->Distance->text();
    if(start.isEmpty())
        ui->textEdit->append(u8"注意飞机ID位空");
    if(distance.isEmpty())
        ui->textEdit->append(u8"飞行距离为空");

    uint8_t sbuf[512] = {0};
    hgprotocol_message_t  msg;
    hgprotocol_lcommand_t flydata;
    memset(&flydata, 0, sizeof(hgprotocol_lcommand_t));
    flydata.cmd = MAV_CMD_BACK;
    flydata.start_id = start.toUShort();
    flydata.end_id = start.toUShort();
    flydata.param[0] = distance.toInt();
    hgprotocol_lcommand_encode(0, 0, &msg, &flydata);
    int size = hgprotocol_msg_to_send_buffer(sbuf, &msg);

    senddata(sbuf, size, flydata.start_id);
    ui->textEdit->append( QString(u8"飞机:%1 向后已发送").arg(flydata.start_id));
}
/*--------------------------------------------------------------------------*/


/*------------------------------向前-----------------------------------------*/
void MainWindow::DFlyFront()
{
    if(temporaryR != rowbuff || temporaryC != colbuff)
    {
        ui->tableRobot->item(temporaryR, temporaryC)->setBackground(QPixmap(":/qss/fly.png"));
        temporaryR = rowbuff;
        temporaryC = colbuff;
        ui->tableRobot->item(rowbuff, colbuff)->setBackground(QPixmap(":/qss/fly_on.png")) ;
    }
    else
        ui->tableRobot->item(rowbuff, colbuff)->setBackground(QPixmap(":/qss/fly_on.png")) ;
    QString start =  ui->Editnum->text();
    QString distance = ui->Distance->text();
    if(start.isEmpty())
        ui->textEdit->append(u8"注意飞机ID位空");
    if(distance.isEmpty())
        ui->textEdit->append(u8"飞行距离为空");

    uint8_t sbuf[512] = {0};
    hgprotocol_message_t  msg;
    hgprotocol_lcommand_t flydata;
    memset(&flydata, 0, sizeof(hgprotocol_lcommand_t));
    flydata.cmd = MAV_CMD_FORWARD;
    flydata.start_id = start.toUShort();
    flydata.end_id = start.toUShort();
    flydata.param[0] = distance.toInt();
    hgprotocol_lcommand_encode(0, 0, &msg, &flydata);
    int size = hgprotocol_msg_to_send_buffer(sbuf, &msg);

    senddata(sbuf, size, flydata.start_id);
    ui->textEdit->append( QString(u8"飞机:%1 向前已发送").arg(flydata.start_id));
}
/*--------------------------------------------------------------------------*/


/*------------------------------向左-----------------------------------------*/
void MainWindow::DFlyLeft()
{
    if(temporaryR != rowbuff || temporaryC != colbuff)
    {
        ui->tableRobot->item(temporaryR, temporaryC)->setBackground(QPixmap(":/qss/fly.png"));
        temporaryR = rowbuff;
        temporaryC = colbuff;
        ui->tableRobot->item(rowbuff, colbuff)->setBackground(QPixmap(":/qss/fly_on.png")) ;
    }
    else
        ui->tableRobot->item(rowbuff, colbuff)->setBackground(QPixmap(":/qss/fly_on.png")) ;
    QString start =  ui->Editnum->text();
    QString distance = ui->Distance->text();
    if(start.isEmpty())
        ui->textEdit->append(u8"注意飞机ID位空");
    if(distance.isEmpty())
        ui->textEdit->append(u8"飞行距离为空");

    uint8_t sbuf[512] = {0};
    hgprotocol_message_t  msg;
    hgprotocol_lcommand_t flydata;
    memset(&flydata, 0, sizeof(hgprotocol_lcommand_t));
    flydata.cmd = MAV_CMD_LEFT;
    flydata.start_id = start.toUShort();
    flydata.end_id = start.toUShort();
    flydata.param[0] = distance.toInt();
    hgprotocol_lcommand_encode(0, 0, &msg, &flydata);
    int size = hgprotocol_msg_to_send_buffer(sbuf, &msg);

    senddata(sbuf, size, flydata.start_id);
    ui->textEdit->append( QString(u8"飞机:%1 向左已发送").arg(flydata.start_id));
}
/*--------------------------------------------------------------------------*/


/*------------------------------向右-----------------------------------------*/
void MainWindow::DFlyRight()
{
    if(temporaryR != rowbuff || temporaryC != colbuff)
    {
        ui->tableRobot->item(temporaryR, temporaryC)->setBackground(QPixmap(":/qss/fly.png"));
        temporaryR = rowbuff;
        temporaryC = colbuff;
        ui->tableRobot->item(rowbuff, colbuff)->setBackground(QPixmap(":/qss/fly_on.png")) ;
    }
    else
        ui->tableRobot->item(rowbuff, colbuff)->setBackground(QPixmap(":/qss/fly_on.png")) ;
    QString start =  ui->Editnum->text();
    QString distance = ui->Distance->text();
    if(start.isEmpty())
        ui->textEdit->append(u8"注意飞机ID位空");
    if(distance.isEmpty())
        ui->textEdit->append(u8"飞行距离为空");

    uint8_t sbuf[512] = {0};
    hgprotocol_message_t  msg;
    hgprotocol_lcommand_t flydata;
    memset(&flydata, 0, sizeof(hgprotocol_lcommand_t));
    flydata.cmd = MAV_CMD_RIGHT;
    flydata.start_id = start.toUShort();
    flydata.end_id = start.toUShort();
    flydata.param[0] = distance.toInt();
    hgprotocol_lcommand_encode(0, 0, &msg, &flydata);
    int size = hgprotocol_msg_to_send_buffer(sbuf, &msg);

    senddata(sbuf, size, flydata.start_id);
    ui->textEdit->append( QString(u8"飞机:%1 向右已发送").arg(flydata.start_id));
}
/*--------------------------------------------------------------------------*/


/*----------------------------断开网络---------------------------------------*/
void MainWindow::Distcp()
{
    QString start =  ui->flystart->text();
    QString end   = ui->flyend->text();
    uint8_t sbuf[512] = {0};
    hgprotocol_message_t  msg;
    hgprotocol_lcommand_t flydata;
    memset(&flydata, 0, sizeof(hgprotocol_lcommand_t));
    flydata.cmd = MAV_CMD_NET_DISCONNECT;
    flydata.start_id = start.toUShort();
    flydata.end_id = end.toUShort();
    flydata.param[0] = 0x01;
    hgprotocol_lcommand_encode(0, 0, &msg, &flydata);
    int size = hgprotocol_msg_to_send_buffer(sbuf, &msg);

    int mun = end.toInt() - start.toInt() + 1;
    for(int i = 0; i < mun; i++)
    {
        flydata.start_id = start.toUShort() + i;
        //flydata.end_id = flydata.start_id;
        senddata(sbuf, size, flydata.start_id);
    }
    Sleep(1500);
}
/*--------------------------------------------------------------------------*/


/*------------------------------拍照-----------------------------------------*/
void MainWindow::DPhotograph()
{
    if(temporaryR != rowbuff || temporaryC != colbuff)
    {
        ui->tableRobot->item(temporaryR, temporaryC)->setBackground(QPixmap(":/qss/fly.png"));
        temporaryR = rowbuff;
        temporaryC = colbuff;
        ui->tableRobot->item(rowbuff, colbuff)->setBackground(QPixmap(":/qss/fly_on.png")) ;
    }
    else
        ui->tableRobot->item(rowbuff, colbuff)->setBackground(QPixmap(":/qss/fly_on.png")) ;
    QString start =  ui->Editnum->text();
    if(start.isEmpty())
        ui->textEdit->append(u8"注意飞机ID位空");

    uint8_t sbuf[512] = {0};
    hgprotocol_message_t  msg;
    hgprotocol_lcommand_t flydata;
    memset(&flydata, 0, sizeof(hgprotocol_lcommand_t));
    flydata.cmd = MAV_CMD_TAKEPHOTO;
    flydata.start_id = start.toUShort();
    flydata.end_id = start.toUShort();
    flydata.param[0] = 0x01;
    hgprotocol_lcommand_encode(0, 0, &msg, &flydata);
    int size = hgprotocol_msg_to_send_buffer(sbuf, &msg);

    //senddata(sbuf, size, flydata.start_id);
    ui->textEdit->append( QString(u8"飞机:%1 拍照已发送").arg(flydata.start_id));
}
/*--------------------------------------------------------------------------*/


/*------------------------------录像-----------------------------------------*/
void MainWindow::DVideo()
{
    if(temporaryR != rowbuff || temporaryC != colbuff)
    {
        ui->tableRobot->item(temporaryR, temporaryC)->setBackground(QPixmap(":/qss/fly.png"));
        temporaryR = rowbuff;
        temporaryC = colbuff;
        ui->tableRobot->item(rowbuff, colbuff)->setBackground(QPixmap(":/qss/fly_on.png")) ;
    }
    else
        ui->tableRobot->item(rowbuff, colbuff)->setBackground(QPixmap(":/qss/fly_on.png")) ;
    QString start =  ui->Editnum->text();
    if(start.isEmpty())
        ui->textEdit->append(u8"注意飞机ID位空");

    uint8_t sbuf[512] = {0};
    hgprotocol_message_t  msg;
    hgprotocol_lcommand_t flydata;
    memset(&flydata, 0, sizeof(hgprotocol_lcommand_t));
    flydata.cmd = MAV_CMD_RECORD;
    flydata.start_id = start.toUShort();
    flydata.end_id = start.toUShort();
    flydata.param[0] = 0x01;
    hgprotocol_lcommand_encode(0, 0, &msg, &flydata);
    int size = hgprotocol_msg_to_send_buffer(sbuf, &msg);

    //senddata(sbuf, size, flydata.start_id);
    ui->textEdit->append( QString(u8"飞机:%1 录像已发送").arg(flydata.start_id));
}
/*--------------------------------------------------------------------------*/


/*----------------------------关闭提示----------------------------------------*/
void MainWindow::closeEvent(QCloseEvent *e)
{
    Distcp();
    //窗口关闭时弹出的提示窗口
    QMessageBox msgBox;
    msgBox.setText(u8"提示");
    msgBox.setInformativeText(u8"确认退出?");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    int ret = msgBox.exec();
    if(ret == QMessageBox::Ok){
        //若用户点击确认，则接收这个事件,当前窗口会关闭
        e->accept();
    }else{
        //若用户点击取消，则忽略这个事件，当前窗口不会关闭
        e->ignore();
    }
}
/*--------------------------------------------------------------------------*/


/*------------------------------悬停-----------------------------------------*/
void MainWindow::DStop()
{
    if(temporaryR != rowbuff || temporaryC != colbuff)
    {
        ui->tableRobot->item(temporaryR, temporaryC)->setBackground(QPixmap(":/qss/fly.png"));
        temporaryR = rowbuff;
        temporaryC = colbuff;
        ui->tableRobot->item(rowbuff, colbuff)->setBackground(QPixmap(":/qss/fly_on.png")) ;
    }
    else
        ui->tableRobot->item(rowbuff, colbuff)->setBackground(QPixmap(":/qss/fly_on.png")) ;
    QString start =  ui->Editnum->text();
    if(start.isEmpty())
        ui->textEdit->append(u8"注意飞机ID位空");

    uint8_t sbuf[512] = {0};
    hgprotocol_message_t  msg;
    hgprotocol_lcommand_t flydata;
    memset(&flydata, 0, sizeof(hgprotocol_lcommand_t));
    flydata.cmd = MAV_CMD_STOP;
    flydata.start_id = start.toUShort();
    flydata.end_id = start.toUShort();
    hgprotocol_lcommand_encode(0, 0, &msg, &flydata);
    int size = hgprotocol_msg_to_send_buffer(sbuf, &msg);

    senddata(sbuf, size, flydata.start_id);
    ui->textEdit->append( QString(u8"飞机:%1 悬停已发送").arg(flydata.start_id));
}
/*--------------------------------------------------------------------------*/


/*------------------------------云台-----------------------------------------*/
void MainWindow::DPTZ(int value)
{
    QString start =  ui->flystart->text();
    if(start.isEmpty())
        ui->textEdit->append(u8"注意飞机ID位空");
    qDebug() << "valueChanged" << value;
    static int cp = 0, lp = 0;
    cp = value;
    uint8_t sbuf[512] = {0};
    hgprotocol_message_t  msg;
    hgprotocol_lcommand_t flydata;
    memset(&flydata, 0, sizeof(hgprotocol_lcommand_t));

    if(value > 0 && value <= 90 && value % 3 == 0)
    {
        flydata.cmd = MAV_CMD_PTZ;
        flydata.start_id = start.toUShort();
        flydata.end_id = start.toUShort();
        if(lp < cp){
            flydata.param[0] = -1500;
            flydata.param[1] = 1;
            hgprotocol_lcommand_encode(0, 0, &msg, &flydata);
            int size = hgprotocol_msg_to_send_buffer(sbuf, &msg);
            senddata(sbuf, size, flydata.start_id);
            ui->textEdit->append( QString(u8"飞机:%1 云台已发送").arg(flydata.start_id));
        }
        else{
            flydata.param[0] = 1500;
            flydata.param[1] = 1;
            hgprotocol_lcommand_encode(0, 0, &msg, &flydata);
            int size = hgprotocol_msg_to_send_buffer(sbuf, &msg);
            senddata(sbuf, size, flydata.start_id);
            ui->textEdit->append( QString(u8"飞机:%1 云台已发送").arg(flydata.start_id));
        }
        lp = cp;
    }
    else if(value < 0 && value >= -90 && value % 3 == 0)
    {
        flydata.cmd = MAV_CMD_PTZ;
        flydata.start_id = start.toUShort();
        flydata.end_id = start.toUShort();
        if(lp < cp){
            flydata.param[0] = 1500;
            flydata.param[1] = 1;
            hgprotocol_lcommand_encode(0, 0, &msg, &flydata);
            int size = hgprotocol_msg_to_send_buffer(sbuf, &msg);
            senddata(sbuf, size, flydata.start_id);
            ui->textEdit->append( QString(u8"飞机:%1 云台已发送").arg(flydata.start_id));
        }
        else{
            flydata.param[0] = -1500;
            flydata.param[1] = 1;
            hgprotocol_lcommand_encode(0, 0, &msg, &flydata);
            int size = hgprotocol_msg_to_send_buffer(sbuf, &msg);
            senddata(sbuf, size, flydata.start_id);
            ui->textEdit->append( QString(u8"飞机:%1 云台已发送").arg(flydata.start_id));
        }
        lp = cp;
    }
}
/*--------------------------------------------------------------------------*/



/*------------------------------授时-----------------------------------------*/
void MainWindow::DTiming()
{
    QDateTime timeDate = QDateTime::currentDateTime();  // 获取当前时间
    int timeStr = timeDate .toTime_t();   				// 将当前时间转为时间戳

    uint8_t sbuf[512] = {0};
    hgprotocol_message_t  msg;
    hgprotocol_lcommand_t flydata;
    memset(&flydata, 0, sizeof(hgprotocol_lcommand_t));
    flydata.cmd = MAV_CMD_TIMESYNC;
    flydata.start_id = 1;
    flydata.end_id = 1;
    flydata.utc = timeStr;
    hgprotocol_lcommand_encode(0, 0, &msg, &flydata);
    int size = hgprotocol_msg_to_send_buffer(sbuf, &msg);

    for(int i = 0; i < 3; i++)
    {
        flydata.start_id = 1 + i;
        //flydata.end_id = flydata.start_id;
        senddata(sbuf, size, flydata.start_id);
        ui->textEdit->append( QString(u8"飞机:%1 授时已发送").arg(flydata.start_id));
    }
}
/*--------------------------------------------------------------------------*/


/*------------------------------起飞3----------------------------------------*/
void MainWindow::DTakeOff3()
{
    uint8_t sbuf[512] = {0};
    hgprotocol_message_t  msg;
    hgprotocol_lcommand_t flydata;
    memset(&flydata, 0, sizeof(hgprotocol_lcommand_t));
    flydata.cmd = MAV_CMD_TAKEOFF;
    flydata.start_id = 1;
    flydata.end_id = 1;
    flydata.param[0] = 1000;
    flydata.param[1] = 24;
    flydata.param[2] = 0x8000|0x4000;
    hgprotocol_lcommand_encode(0, 0, &msg, &flydata);
    int size = hgprotocol_msg_to_send_buffer(sbuf, &msg);

    int mun = 3;
    for(int i = 0; i < mun; i++)
    {
        flydata.start_id = 1 + i;
        //flydata.end_id = flydata.start_id;
        senddata(sbuf, size, flydata.start_id);
        ui->textEdit->append( QString(u8"飞机:%1 起飞已发送").arg(flydata.start_id));
    }
}
/*--------------------------------------------------------------------------*/


/*------------------------------降落-----------------------------------------*/
void MainWindow::DLanding3()
{
    uint8_t sbuf[512] = {0};
    hgprotocol_message_t  msg;
    hgprotocol_lcommand_t flydata;
    memset(&flydata, 0, sizeof(hgprotocol_lcommand_t));
    flydata.cmd = MAV_CMD_LAND;
    flydata.start_id = 1;
    flydata.end_id = 1;
    flydata.param[0] = 0x2000;
    hgprotocol_lcommand_encode(0, 0, &msg, &flydata);
    int size = hgprotocol_msg_to_send_buffer(sbuf, &msg);

    int mun = 3;
    for(int i = 0; i < mun; i++)
    {
        flydata.start_id = 1 + i;
        senddata(sbuf, size, flydata.start_id);
        ui->textEdit->append( QString(u8"飞机:%1 降落已发送").arg(flydata.start_id));
    }
}
/*--------------------------------------------------------------------------*/


/*-----------------------------展示飞机---------------------------------------*/
void MainWindow::FlyStart()
{
    qDebug() << "kke";
    // 设置 QLabel 的位置属性
    // imageLabel->setPixmap(QPixmap(":/qss/fly_on.png"));
    // QPainter painter(ui->flywidget);
    //painter.drawPixmap(50,100,QPixmap(":/qss/fly_on.png"));
    //update();

}
/*--------------------------------------------------------------------------*/


/*--------------------------------动捕跟踪-----------------------------------*/
void MainWindow::DTrack()
{
    qDebug() << "1";
    flag = 1;
}
/*--------------------------------------------------------------------------*/


/*--------------------------------跟踪取消-----------------------------------*/
void MainWindow::DCancel()
{
    flag = 0;
    uint8_t sbuf[512] = {0};

    for(int j = 0; j < 2; j++)
    {
        QDateTime timeDate = QDateTime::currentDateTime();  // 获取当前时间
        int timeStr = timeDate .toTime_t();   				// 将当前时间转为时间戳
        hgprotocol_message_t  msg;
        hgprotocol_swarm_track_t flydata;
        memset(&flydata, 0, sizeof(hgprotocol_swarm_track_t));
        flydata.swarm_state = TAKEOFF_DONE;
        hgprotocol_swarm_track_encode(0, 0, &msg, &flydata);
        int size = hgprotocol_msg_to_send_buffer(sbuf, &msg);

        for(int i = 1; i < 4; i++)
        {
            senddata(sbuf, size, i);
            ui->textEdit->append( QString(u8"飞机:%1 取消已发送").arg(i));
        }
    }
}
/*--------------------------------------------------------------------------*/



/*--------------------------------保存数据-----------------------------------*/
void MainWindow::DSave()
{
    FLY_SET_DATA setdata;
    setdata.radius = ui->EditRadius->text();
    setdata.carnum = ui->EditCar->currentText();
    setdata.direction = ui->BoxDirection->currentText();
    setdata.speed = ui->EditSpeed->text();
    setdata.covering = ui->BoxCovering->currentText();
    setdata.height= ui->EditHeight->text();

    if(setdata.direction == QString("前"))
    {
        setdata.direction = QString("0");
    }
    else if(setdata.direction == QString("右前"))
    {
        setdata.direction = QString("1");
    }
    else if(setdata.direction == QString("右"))
    {
        setdata.direction = QString("2");
    }
    else if(setdata.direction == QString("右后"))
    {
        setdata.direction = QString("3");
    }
    else if(setdata.direction == QString("后"))
    {
        setdata.direction = QString("4");
    }
    else if(setdata.direction == QString("左后"))
    {
        setdata.direction = QString("5");
    }
    else if(setdata.direction == QString("左"))
    {
        setdata.direction = QString("6");
    }
    else if(setdata.direction == QString("左前"))
    {
        setdata.direction = QString("7");
    }
    else if(setdata.direction == QString("中"))
    {
        setdata.direction = QString("8");
    }

    if(setdata.covering == QString("是"))
    {
        setdata.covering = QString("1");
    }
    else if(setdata.covering == QString("否"))
    {
        setdata.covering = QString("0");
    }

    if(rowcolumn == -1)
        return;
    //将设置数据存入map中
    QMap<int, FLY_SET_DATA>::iterator it;
    it = mapSetFlyData.find(rowcolumn);
    if (it != mapSetFlyData.end())    //找到数据
    {
       mapSetFlyData[rowcolumn] = setdata;
    }
    else
    {
        mapSetFlyData.insert(rowcolumn, setdata);
    }
}
/*--------------------------------------------------------------------------*/\


/*---------------------------------1号追捕-----------------------------------*/
void MainWindow::DConfirm1(bool checked)
{
    if (checked)
    {
        qDebug() << "1";
        flyflag1 = 1;
    }
    else
    {
        qDebug() << "0";
        flyflag1 = 0;
    }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------2号追捕-----------------------------------*/
void MainWindow::DConfirm2(bool checked)
{

    if (checked)
    {
        qDebug() << "1";
        flyflag2 = 1;
    }
    else
    {
        qDebug() << "0";
        flyflag2 = 0;
    }
}
/*--------------------------------------------------------------------------*/


/*--------------------------------3号追捕-----------------------------------*/
void MainWindow::DConfirm3(bool checked)
{
    if (checked)
    {
        qDebug() << "1";
        flyflag3 = 1;
    }
    else
    {
        qDebug() << "0";
        flyflag3 = 0;
    }
}
/*--------------------------------------------------------------------------*/


/*--------------------------------视觉追捕-----------------------------------*/
void MainWindow::DVision()
{
    qDebug() << "2";
    flag = 2;
}
/*--------------------------------------------------------------------------*/


/*------------------------------降落-----------------------------------------*/
void MainWindow::DLanding4(int id)
{
    uint8_t sbuf[512] = {0};
    hgprotocol_message_t  msg;
    hgprotocol_lcommand_t flydata;
    memset(&flydata, 0, sizeof(hgprotocol_lcommand_t));
    flydata.cmd = MAV_CMD_LAND;
    flydata.start_id = id;
    flydata.end_id = id;
    flydata.param[0] = 0x2000;
    hgprotocol_lcommand_encode(0, 0, &msg, &flydata);
    int size = hgprotocol_msg_to_send_buffer(sbuf, &msg);

    senddata(sbuf, size, flydata.start_id);
    ui->textEdit->append( QString(u8"飞机:%1 降落已发送").arg(flydata.start_id));

}
/*--------------------------------------------------------------------------*/
