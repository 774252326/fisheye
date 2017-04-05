#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

//    c.open("Z:\\Project Share Folder\\JIAN\\Entaniya Fisheye 280 on GoPro HERO 4 Direct mount.mp4");

    c.open( 0 );
    c.set(cv::CAP_PROP_FRAME_WIDTH, 1280);
    c.set(cv::CAP_PROP_FRAME_HEIGHT, 1024);

    startTimer(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::timerEvent(QTimerEvent *event)
{
    cv::Mat image;
    if(!c.read(image))
    {
        killTimer(0);
        c.release();
        return;
    }

//    image=cv::imread("C:\\Users\\jhanbin\\Pictures\\untitled.png");
//    killTimer(0);

    QImage mRenderQtImg = QImage((const unsigned char*)(image.data),
                                 image.cols, image.rows,
                                 image.step1(), QImage::Format_RGB888).mirrored().rgbSwapped();

    // Do what you want with the image :-)

    // Show the image
    ui->openGLWidget->showImage(mRenderQtImg);
}
