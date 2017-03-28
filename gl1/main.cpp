#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    cv::imshow("m",cv::imread("C:\\Users\\jhanbin\\Documents\\Visual Studio 2015\\Projects\\ConsoleApplication2\\ConsoleApplication2\\or.png", 0)>128);
    return cv::waitKey();

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
