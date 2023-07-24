#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QDir>
#include <QSettings>
#include <QDebug>
#include <QPushButton>
#include <QPainter>
#include <QInputDialog>
#include <QtMath>
#include <iostream>

#include "videoplayer.h"
#include "yolo/yolo.h"
#include <iostream>
#include <csignal>
#include <opencv4/opencv2/opencv.hpp>

#include <iostream>
#include <iomanip>
#include <ctime>
#include <opencv2/opencv.hpp>

#include "tracker/utils.h"
#include "tracker/kalmanboxtracker.h"
#include "tracker/sort.h"

#include<algorithm>
#include<vector>
#include<iostream>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
signals:
    void sig_fame(QImage img);

private:
    Ui::Widget *ui;
private:
    void readconfig();
    QString rtspaddr;
    void initWidget();
    void initconnect();
private slots:
    void slot_open_or_close();

protected:
    //void paintEvent(QPaintEvent *event);

private:
    VideoPlayer *mPlayer; //播放线程
    QImage mImage; //记录当前的图像
    QString url;
    //QImage initimage;
    void initYolo();
    cv::Mat QImage2Mat(QImage image);
    QImage Mat2QImage(const cv::Mat &mat);
    Yolo *myyolo;
    sort *mot_tracker;

    void readvideo();

    void filter(float* Boxes,int* ClassIndexs,int* BboxNum,float* Scores,
                float conf_thres, std::vector<int> classes,float *newBoxes,
                int *newClassIndexs,int *newBboxNum,float *newScores);

    //std::vector<cv::Vec3b> colors(32);
    //cv::VideoWriter outputVideo;
    int encode_type ;//= VideoWriter::fourcc('M', 'J', 'P', 'G');
    std::vector<cv::Vec3b> colors;

    //cv::VideoWriter outputVideo;


private slots:
    void slotGetOneFrame(QImage img);
};
#endif // WIDGET_H
