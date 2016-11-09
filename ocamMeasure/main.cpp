
//#include "stereoocam.hpp"
#include "ocamcv.hpp"


#include <QtCore>
#include <QSemaphore>


class dtc : public OcamCV
{
public:
    cv::VideoCapture c;
    cv::Mat m[2];
    int k;
    int idx;
    dtc(std::string path)
        : OcamCV(path)
        , c(0)
        , k(0)
        , idx(0)
    {

    }

    void Capture()
    {
        c.read(m[1-idx]);
        idx=1-idx;
    }
    void Process()
    {

        clock_t t=clock();
        cv::Vec3d tt=MarkerPos(m[idx],12);

        std::cout<<(float)(clock()-t)*1000/CLOCKS_PER_SEC<<"ms\n"<<std::flush;

        cv::imshow("f",m[idx]);
        k=cv::waitKey(1);
    }
};


class CaptureThread : public QThread
{

public:

    std::string name;
    QSemaphore *sc;
    QSemaphore *sp;
    dtc *p;
    CaptureThread(QSemaphore *sc0, QSemaphore *sp0, dtc *p0)
    {
        sc=sc0;
        sp=sp0;
        p=p0;
    }

    void run()
    {
        while(p->k!=27)
        {
            sp->acquire();
            std::cout<<name<<"   "<<"\n"<<std::flush;

            //            p->c.read(p->m[1-p->idx]);
            //            p->idx=1-p->idx;

            p->Capture();

            sc->release();

        }
    }
};


class ProcessThread : public QThread
{

public:

    std::string name;
    QSemaphore *sc;
    QSemaphore *sp;
    dtc *p;
    ProcessThread(QSemaphore *sc0, QSemaphore *sp0, dtc *p0)
    {
        sc=sc0;
        sp=sp0;
        p=p0;
    }

    void run()
    {
        cv::namedWindow("f",cv::WINDOW_NORMAL);

        while(p->k!=27)
        {
            sc->acquire();
            std::cout<<name<<"   a"<<"\n"<<std::flush;

            //            cv::Mat mg;
            //            cv::cvtColor(p->m[p->idx],mg,CV_BGR2GRAY);

            //            cv::imshow("f",mg);

            //            p->k=cv::waitKey(1);
            p->Process();

            sp->release();
        }
    }
};

static void Test()
{
    dtc d("2.1mm/calib_results800x600.txt");

    d.model.ScaleModel(0.4);

    d.c.set(cv::CAP_PROP_FRAME_WIDTH, 320);
    d.c.set(cv::CAP_PROP_FRAME_HEIGHT,240);
    d.c.set(cv::CAP_PROP_SETTINGS,1);

    d.Capture();

    QSemaphore sc(1);
    QSemaphore sp(1);

    ProcessThread mt(&sc,&sp,&d);

    mt.name="process";
    CaptureThread mt2(&sc,&sp,&d);

    mt2.name="capture";

    mt2.start();

    mt.start();

    //    QThread::sleep(1000);
}


int main(int argc, char **argv)
{
#if 1

    if(argc!=3)
    {
        OcamCV("2.1mm/calib_results1920x1080.txt").test2(1280,720,2.0/3);
        return 0;
    }
    double length;
    std::istringstream(argv[1])>>length;
    std::string port(argv[2]);
    std::cout<<length<<'\n'<<port<<'\n';
    OcamCV("2.1mm/calib_results1920x1080.txt").test2(1280,720,2.0/3,length,port);
    return 0;
#endif

    //        test();
    //Test();

#if 0
    if(argc!=3)
    {
        StereoOcam::DualCamTest3();
        return 0;
    }


    double th1;
    double th2;
    std::istringstream(argv[1])>>th1;
    std::istringstream(argv[2])>>th2;
    StereoOcam::DualCamTest3(th1,th2);
#endif


    return 0;
}
