
#ifndef OCAMCV_HPP
#define OCAMCV_HPP
#include <opencv2/opencv.hpp>

#include "findmarkerfisheye.hpp"

#include "ToArduino.hpp"

#include "noticethread.hpp"

#include "fiocpp.hpp"
#include "fioqt.hpp"

#include "controlthread.hpp"

//#include "depththread.hpp"

//#define RECORDIMG
#define OUTADN
//#define SOUND
//#define DEPTHS

class OcamCV
{
public:
    FisheyeModel model;
    FindMarkerFisheye fmf;
public:
    OcamCV(std::string path)
        : model(path.c_str())
        , fmf(0)
    {

    }


    void test3(int w, int h, double scale,
               double markerLength=120, std::string port="/dev/ttyACM0")
    {
        model.Scale(scale);

        cv::Mat m;

        cv::VideoCapture c(0);

        c.set(cv::CAP_PROP_FRAME_WIDTH, w);
        c.set(cv::CAP_PROP_FRAME_HEIGHT,h);
//        c.set(cv::CAP_PROP_SETTINGS,1);

        cv::namedWindow("f",cv::WINDOW_NORMAL);

        cv::waitKey(20000);

        c.read(m);
        //        cv::imwrite("capture.png",m);

        QSemaphore s(1);



#ifdef OUTADN
        ControlThread ct(&s);
        ct.port=port;
        ct.start();
#ifdef DEPTHS
        DepthThread dt(&ct);
        dt.start();
#endif
#endif


#ifdef RECORDIMG
        std::string folder=TimeString();
        SetFolder(folder);
#endif

#ifdef SOUND
        NoticeThread mt;
        mt.start();

        bool init=false;



#endif


//        std::cout<<"11\n"<<std::flush;
        int k=0;
        while(k!=27)
        {

            if(c.read(m))
            {
//                clock_t t=clock();
#ifdef RECORDIMG
                cv::Mat m1;
                m.copyTo(m1);
#endif

                cv::Vec3d tt=fmf.Detect(2,m,model,markerLength);

#ifdef SOUND
                if(!init && tt[2]>0)
                    init=true;
#endif

                fmf.DrawResult(m,model,markerLength);
                std::cout<<tt<<'\n'<<std::flush;

#ifdef RECORDIMG

                if(tt[2]>0)
                {
                    std::string name=TimeString();
                    cv::imwrite(folder+"/"+name+".png",m1);
                    cv::imwrite(folder+"/"+name+"r.png",m);
                }
#endif

#ifdef OUTADN
                //                s.acquire();
                //                ct.pos[0]=tt[0];
                //                ct.pos[1]=tt[1];
                //                ct.pos[2]=tt[2];
                //                ct.dirtyflag=true;
                //                s.release();
                ct.SetPos(tt);
#endif
//                std::cout<<(float)(clock()-t)*1000/CLOCKS_PER_SEC<<"ms\n\n\n"<<std::flush;
#ifdef SOUND
                if(init)
                {

                if(tt[2]>0)
                    if(tt[2]<4000)
                        mt.index=2;
                    else
                        mt.index=0;
                else
                    mt.index=1;
                }
                else
                {
                    mt.index=2;
                }
#endif
                cv::imshow("f",m);
                k=cv::waitKey(1);

            }
            else
            {
                std::cout<<"fail read image\n"<<std::flush;
#ifdef OUTADN
                ct.SetPos(cv::Vec3d(0,0,-1));
#endif
#ifdef SOUND
                mt.index=1;
#endif
            }
        }
#ifdef SOUND
        mt.index=-1;
#endif
#ifdef OUTADN
        ct.goflag=false;
#ifdef DEPTHS
        dt.goflag=false;
#endif
#endif

    cv::waitKey(1000);

    }




};

#endif // OCAMCV_HPP
