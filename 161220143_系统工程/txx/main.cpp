#include "mainwindow.h"
#include <QApplication>
#include <iostream>

//#include <opencv2/highgui/highgui_c.h>

using namespace std;
//using namespace cv;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
    /**QApplication a(argc, argv);
    Mat image= imread("./1.jpg");
    cvNamedWindow("window", CV_WINDOW_AUTOSIZE);
    if (!image.empty()) {
        imshow("window", image);
    }
    waitKey(0);
    return a.exec();*/
}
