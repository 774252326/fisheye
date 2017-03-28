#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    c.open( 1 );
//    c.set(cv::CAP_PROP_FRAME_WIDTH, 2592);
//    c.set(cv::CAP_PROP_FRAME_HEIGHT, 1944);

    startTimer(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::timerEvent(QTimerEvent *event)
{
    cv::Mat image;
    c >> image;

//    std::cout<<image.size()<<std::flush;

    QImage mRenderQtImg = QImage((const unsigned char*)(image.data),
                                 image.cols, image.rows,
                                 image.step1(), QImage::Format_RGB888).mirrored().rgbSwapped();

    // Do what you want with the image :-)

    // Show the image
    ui->openGLWidget->showImage(mRenderQtImg);
}
