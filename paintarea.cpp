#include "paintarea.h"
#include "ui_paintarea.h"
#include "include.h"

paintarea::paintarea(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::paintarea)
{
    ui->setupUi(this);
    this->setGeometry(0, 0, 800, 480);
    setPalette(QPalette(Qt::white));
    setAutoFillBackground(true);

    /* 定时器实例化 */
    timer = new QTimer(this);

    /* 定时100ms */
    timer->start(500);

    /* 信号槽连接 */
    //connect(timer, SIGNAL(timeout()), this, SLOT(timerTimeOut()));
    connect(timer, &QTimer::timeout, this, &paintarea::timerTimeOut);
}

paintarea::~paintarea()
{
    delete ui;
}


QPoint readyapply,prereadyapply,afterapply=QPoint(0,0);

void paintarea::paintEvent(QPaintEvent *event)
{

//    // 绘图
//    QPainter painter(this);
//    pixfly.load(":/qss/fly_ui.png");
//    painter.drawPixmap(50, 750, 50, 50, pixfly);
//    painter.drawPixmap(100, 750, 50, 50, pixfly);
//    painter.drawPixmap(150, 750, 50, 50, pixfly);
//    pixcar.load(":/qss/car.png");
//    painter.drawPixmap(150, 50, 80, 50, pixcar);
//    painter.drawPixmap(650, 50, 80, 50, pixcar);

//    // 利用更改坐标原点实现平移
//    painter.translate(100,100);     //将（100，100）设为坐标原点
//    painter.drawPixmap(0,0,50,50,pix);  //实现缩放
//    painter.translate(-100, -100);  //让图片的中心作为旋转的中心

//        // 实现图片的缩放
//        painter.translate(100,100); //将（100，100）设为坐标原点
//        qreal width = pix.width(); //获得以前图片的宽和高
//        qreal height = pix.height();

//        pix = pix.scaled(width*2,height*2,Qt::KeepAspectRatio);
//        //将图片的宽和高都扩大两倍，并且在给定的矩形内保持宽高的比值
//        painter.drawPixmap(100,100,pix);
//        painter.translate(-100, -100); //让图片的中心作为旋转的中心

//        // 实现图片的旋转
//        painter.translate(500, 400); //让图片的中心作为旋转的中心
//        painter.rotate(90); //顺时针旋转90度
//        painter.drawPixmap(0,0,100,100,pix);
//        painter.rotate(-90); //顺时针旋转90度
//        painter.translate(-500, -400); //让图片的中心作为旋转的中心


}

void paintarea::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        // 获取鼠标点击的位置
        QPoint pos = event->pos();

        // 判断鼠标点击位置是否在绘图区域内
        QRect targetRect;//(100, 100, 200, 200); // 获取绘图区域
        targetRect = pixfly.rect();
        if (targetRect.contains(pos))
        {
            qDebug() << "鼠标点击在绘图区域内";
        }
        else
        {
            qDebug() << "鼠标点击在绘图区域外";
        }

    }

   QWidget::mousePressEvent(event); // 调用基类的鼠标按下事件
}

void paintarea::timerTimeOut()
{
    /* 需要更新界面，不设置不更新 */
    this->update();
}



