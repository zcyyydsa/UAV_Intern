#ifndef PAINTAREA_H
#define PAINTAREA_H

#include <QWidget>

namespace Ui {
class paintarea;
}

class paintarea : public QWidget
{
    Q_OBJECT

public:
    explicit paintarea(QWidget *parent = nullptr);
    ~paintarea();
    void SetFly();
    void slotDisplayPlaneInfo(int row, int column);
protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
private:
    Ui::paintarea *ui;
    QTimer *timer;      //定时器，用于定时更新界面
    QPixmap pixfly;
    QPixmap pixcar;
    int     rowbuff;
    int     colbuff;
    int       rowcolumn = -1;
private slots:
    void timerTimeOut();


};

#endif // PAINTAREA_H
