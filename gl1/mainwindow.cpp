#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

//    c.open("Z:\\Project Share Folder\\JIAN\\Entaniya Fisheye 280 on GoPro HERO 4 Direct mount.mp4");

//    c.open( 1 );
//    c.set(cv::CAP_PROP_FRAME_WIDTH, 1280);
//    c.set(cv::CAP_PROP_FRAME_HEIGHT, 1024);

    timer=startTimer(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::timerEvent(QTimerEvent *event)
{
    std::cout<<"timerEvent\n"<<std::flush;

    cv::Mat image;
//    if(!c.read(image))
    {
//        killTimer(timer);
//        c.release();
//        return;
    }

    image=cv::imread("C:\\Users\\jhanbin\\Desktop\\pro\\fisheye\\gl1\\280_003.jpg");
    killTimer(timer);

    QImage mRenderQtImg = QImage((const unsigned char*)(image.data),
                                 image.cols, image.rows,
                                 image.step1(), QImage::Format_RGB888).mirrored().rgbSwapped();

    // Do what you want with the image :-)

    // Show the image
    ui->openGLWidget->showImage(mRenderQtImg);
}
