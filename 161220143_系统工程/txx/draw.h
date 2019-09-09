#define _CHOOSERECT_ -2
#define _CHOOSE_ -1
#define _MOVE_ 0
#define _POINT_ 1
#define _LINE_ 2
#define _CURVE_ 3
#define _RECT_ 4
#define _FILLEDRECT_ 5
#define _POLY_ 6
#define _FILLEDPOLY_ 7
#define _CIRCLE_ 8
#define _FILLEDCIRCLE_ 9
#define _OVAL_ 10
#define _FILLEDOVAL_ 11

#ifndef DRAW_H
#define DRAW_H

#include <QColor>
#include <QImage>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMouseEvent>
#include <QPainter>
#include <QString>
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
//#include <opencv2/opencv.hpp>
#include <QMessageBox>
#include <windows.h>
#include <glut.h>
#include <QDebug>
#include <math.h>
#include <QPalette>

struct Edge{
    double Ymax;    //边的最大Y值
    double X;       //边的下端点的x坐标或与扫描线交点的x坐标
    double delta_x; //当前扫描线到下一条扫描线之间的x增量（dX/dY)即斜率倒数
    Edge *next;     //下条边
};

struct onePoint{    //每个点的属性
    double x;       //x坐标
    double y;       //y坐标
    int color[3];   //画笔颜色
    int size;  //画笔粗细
    int pid;   //属于哪个图案
    bool isChosen;  //是否被选定
};

class Draw : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
private:
    int currentColor[3];//目前选择颜色
    int currentMode;    //目前画图模式
    int penSize;        //宽度
    int curPicID;       //目前在画的图的编号

    double from_x;
    double from_y;      //移动起始点坐标
    double mid_x;
    double mid_y;

    bool LeftButtonPressed;     //左键是否被按下
    bool RightButtonPressed;    //右键是否被按下
    bool isChoosingPoints;      //是否正在选择区域
    bool isNewChosen;           //是否新选择了一块区域
    bool PolyDrawn;             //是否多边形已经画好

    std::vector<onePoint> allPoints;    //所有我画的点
    std::vector<onePoint> trashPoints;  //被撤销的点
    std::vector<onePoint> chosenPoints; //所有被选中的点
    std::vector<std::pair<double, double>> drawingPoints; //正在画的多边形的所有顶点
public:
    QImage *image=new QImage(1036,501, QImage::Format_RGB888);
    Draw(QWidget *parent = 0);
    ~Draw();
    void setColor(int r,int g,int b);
    void setMode(int i);
    void setpenSize(int i);
    //void paintEvent(QPaintEvent *e);

    int C(int n,int k);
    void initializeGL();
    void resizeGL(int width,int height);
    void paintGL();

    void Undo();
    void Redo();
    void clean_allPoints()
    {
        allPoints.clear();
    }
    void clean_trashPoints()
    {
        trashPoints.clear();
    }
    void clean_chosenPoints()
    {
        chosenPoints.clear();
    }
    void clean_drawingPoints()
    {
        drawingPoints.clear();
    }

    void drawPoint(double x,double y);
    void drawLine(double x1, double y1, double x2, double y2);
    void drawCurve();
    void drawRect(double x1, double y1, double x2, double y2);
    void drawFilledRect(double x1, double y1, double x2, double y2);
    void drawPoly();
    void drawFilledPoly();
    void drawCircle8(double x,double y,double delta_x,double delta_y);
    void drawCircle(double x,double y,double r);
    void drawFilledCircle(double x,double y,double r);
    void drawOval4(double x,double y,double delta_x,double delta_y);
    void drawOval(double x,double y,double a,double b);
    void drawFilledOval(double x,double y,double a,double b);

    void choose(double x,double y);
    void chooseRect(double x1, double y1, double x2, double y2);
    void choose_cancel();
    void move(double delta_x, double delta_y);
    void rotate(double angle);
    void zoom(double times);
    void changeColor();
    void changeSize();

    void openFile(QString filepath);
    void saveFile(QString filepath);
    void newFile();

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent* event);
};
#endif // DRAW_H
