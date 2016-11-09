
#ifndef OCAMCV_HPP
#define OCAMCV_HPP
#include <opencv2/opencv.hpp>
//#include "ocam_functions.h"
#include "ocam_model.hpp"
//#include "Head2Base.hpp"
#include <opencv2/aruco.hpp>
#include "ToArduino.hpp"

#include "noticethread.hpp"

#include "fiocpp.hpp"
#include "fioqt.hpp"

#include "controlthread.hpp"

#include "depththread.hpp"

//#define RECORDIMG
#define OUTADN
//#define SOUND
#define DEPTHS

class OcamCV
{
public:
    ocam_model model;
public:
    OcamCV(std::string path)
        : model(path.c_str())
    {

    }


    cv::Point3d Cam2World(cv::Point2d pixel)
    {
        double point2D[2]={pixel.y,pixel.x};
        double point3D[3];
        model.cam2world(point3D, point2D);
        return cv::Point3d(point3D[1],point3D[0],-point3D[2]);
    }

    static void ComputeTransform(cv::Vec3d &xyz, cv::Vec3d &rxyz, const cv::Vec3d ptFrom[4], const cv::Vec3d ptTo[4])
    {
        cv::Vec3d ctFrom = (ptFrom[0] + ptFrom[1] + ptFrom[2] + ptFrom[3]) / 4;
        cv::Vec3d ctTo = (ptTo[0] + ptTo[1] + ptTo[2] + ptTo[3]) / 4;
        cv::Matx33d H = (ptFrom[0] - ctFrom)*((ptTo[0] - ctTo).t());
        H += (ptFrom[1] - ctFrom)*((ptTo[1] - ctTo).t());
        H += (ptFrom[2] - ctFrom)*((ptTo[2] - ctTo).t());
        H += (ptFrom[3] - ctFrom)*((ptTo[3] - ctTo).t());
        cv::Mat U, S, Vt;
        cv::SVDecomp(H, S, U, Vt);
        cv::Mat R0 = (U*Vt).t();
        cv::Matx33d R((double*)R0.ptr());
        cv::Rodrigues(R, rxyz);
        xyz = ctTo - R*ctFrom;
    }

    static cv::Mat ComputePureRotateM(const cv::Vec3d ptFrom[3], const cv::Vec3d ptTo[3])
    {
        cv::Vec3d ctFrom = (ptFrom[0] + ptFrom[1] + ptFrom[2]) / 4;
        cv::Vec3d ctTo = (ptTo[0] + ptTo[1] + ptTo[2]) / 4;
        cv::Matx33d H = (ptFrom[0] - ctFrom)*((ptTo[0] - ctTo).t());
        H += (ptFrom[1] - ctFrom)*((ptTo[1] - ctTo).t());
        H += (ptFrom[2] - ctFrom)*((ptTo[2] - ctTo).t());
        H += (ctFrom)*(ctTo.t());
        cv::Mat U, S, Vt;
        cv::SVDecomp(H, S, U, Vt);
        cv::Mat R0 = (U*Vt).t();
        return R0;
    }


    void SolvePNPFisheye(const std::vector<cv::Point2d> &pixel,
                         const std::vector<cv::Point3d> &point,
                         cv::Vec3d &Rv,
                         cv::Vec3d &Tv)
    {
        std::vector<cv::Point3d> pixel3dunit(pixel.size(),cv::Point3d());

        cv::Vec3d newz(0,0,0);

        for(size_t i=0;i<pixel.size();i++)
        {
            pixel3dunit[i]=Cam2World(pixel[i]);
            newz=newz+cv::Vec3d(pixel3dunit[i]);
        }

        double Anewz=cv::norm(newz);

        newz=newz/Anewz;

        cv::Vec3d newx=cv::Vec3d(0,1,0).cross(newz);

        double Anewx=cv::norm(newx);
        newx=newx/Anewx;

        cv::Vec3d newy=newz.cross(newx);


        cv::Vec3d p1[3] = {
            cv::Vec3d(1,0,0),
            cv::Vec3d(0,1,0),
            cv::Vec3d(0,0,1)
        };

        cv::Vec3d p0[3] = {
            newx,
            newy,
            newz
        };

        cv::Mat R=ComputePureRotateM(p0,p1);

        cv::Matx33d R33((double*)R.ptr());

        //        std::cout<<"R33="<<R33<<"\n"<<std::flush;


        for(size_t i=0;i<pixel3dunit.size();i++)
        {
            pixel3dunit[i]=R33*pixel3dunit[i];
        }


        cv::Mat K=cv::Mat::eye(3,3,R.type());
        cv::Mat D(5,1,R.type(),cv::Scalar(0));

        std::vector<cv::Point2d> pixelw(pixel.size(),cv::Point2d());

        cv::projectPoints(pixel3dunit,cv::Vec3d(0,0,0),cv::Vec3d(0,0,0),K,D,pixelw);

        cv::Mat rv;
        cv::Mat tv;

        cv::solvePnP(point,pixelw,K,D,rv,tv);

        cv::Mat rvf;
        cv::Mat tvf;

        cv::Mat rv2;
        cv::Rodrigues(R.inv(),rv2);
        cv::Mat tv2(3,1,rv2.type(),cv::Scalar(0));

        cv::composeRT(rv,tv,rv2,tv2,rvf,tvf);

        Rv=cv::Vec3d((double*)rvf.ptr());
        Tv=cv::Vec3d((double*)tvf.ptr());

        std::cout<<"RT="<<Rv<<Tv<<"\n"<<std::flush;
    }


    static void FindAR(const cv::Mat &m, std::vector<cv::Point2f> &pt)
    {
        cv::Ptr<cv::aruco::DetectorParameters> detectorParams = cv::aruco::DetectorParameters::create();
        detectorParams->doCornerRefinement = true; // do corner refinement in markers
        int dictionaryId = 0;
        cv::Ptr<cv::aruco::Dictionary> dictionary =
                cv::aruco::getPredefinedDictionary(cv::aruco::PREDEFINED_DICTIONARY_NAME(dictionaryId));


        std::vector< int > ids;
        std::vector< std::vector< cv::Point2f > > corners, rejected;
        //        clock_t t=clock();
        // detect markers and estimate pose
        cv::aruco::detectMarkers(m, dictionary, corners, ids, detectorParams, rejected);
        //        std::cout<<(float)(clock()-t)*1000/CLOCKS_PER_SEC<<"ms\n"<<std::flush;
        if(corners.empty() || ids[0]!=2)
            pt.clear();
        else
        {
            pt.assign(corners[0].begin(),corners[0].end());
            //            std::cout<<"id="<<ids[0]<<'\n';
        }
    }

    cv::Vec3d MarkerPos(const cv::Mat &m, double length)
    {
        std::vector<cv::Point2f> pts;
        //clock_t t=clock();

        FindAR(m,pts);

        //        std::cout<<(float)(clock()-t)*1000/CLOCKS_PER_SEC<<"ms\n"<<std::flush;

        if(!pts.empty())
        {
            cv::aruco::drawDetectedMarkers(m,std::vector< std::vector<cv::Point2f> >(1,pts));

            std::vector<cv::Point2d> ptsd(pts.size(),cv::Point2d());
            for(size_t i=0;i<pts.size();i++)
            {
                ptsd[i]=pts[i];
            }

            std::vector<cv::Point3d> pt3d(4,cv::Point3d(0,0,0));
            pt3d[1].x=length;
            pt3d[2].x=length;
            pt3d[2].y=length;
            pt3d[3].y=length;


            cv::Vec3d r,t;

            SolvePNPFisheye(ptsd,pt3d,r,t);
            return t;
        }
        return cv::Vec3d(0,0,-1);

    }

    void test()
    {
        //        cv::Mat m=cv::imread("C:\\Users\\jhanbin\\Pictures\\TV_CAM_device_20161005_173453.757.png");

        cv::Mat m=cv::imread("C:\\Users\\jhanbin\\Pictures\\TV_CAM_device_20161005_173411.718.png");

        std::vector<cv::Point2f> pts;

        FindAR(m,pts);

        std::vector<cv::Point2d> ptsd(pts.size(),cv::Point2d());
        for(size_t i=0;i<pts.size();i++)
        {
            ptsd[i]=pts[i];
        }

        std::vector<cv::Point3d> pt3d(4,cv::Point3d(0,0,0));
        pt3d[1].x=12;
        pt3d[2].x=12;
        pt3d[2].y=12;
        pt3d[3].y=12;


        cv::Vec3d r,t;

        SolvePNPFisheye(ptsd,pt3d,r,t);


    }


    void test2(int w, int h, double scale,
               double markerLength=120, std::string port="/dev/ttyACM0")
    {
        model.ScaleModel(scale);

        cv::Mat m;

        cv::VideoCapture c(0);

        c.set(cv::CAP_PROP_FRAME_WIDTH, w);
        c.set(cv::CAP_PROP_FRAME_HEIGHT,h);
        c.set(cv::CAP_PROP_SETTINGS,1);

        cv::namedWindow("f",cv::WINDOW_NORMAL);

        c.read(m);
        cv::imwrite("capture.png",m);
#ifdef OUTADN
        FILE *file = fopen(port.c_str(), "w");
#endif

#ifdef RECORDIMG
        std::string folder=TimeString();
        SetFolder(folder);
#endif

#ifdef SOUND
        NoticeThread mt;
        mt.start();
#endif
        int k=0;
        while(k!=27)
        {

            if(c.read(m))
            {
                clock_t t=clock();
#ifdef RECORDIMG
                cv::Mat m1;
                m.copyTo(m1);
#endif


                cv::Vec3d tt=MarkerPos(m,markerLength);

#ifdef RECORDIMG

                if(tt[2]>0)
                {
                    std::string name=TimeString();
                    cv::imwrite(folder+"/"+name+".png",m1);
                    cv::imwrite(folder+"/"+name+"r.png",m);
                }


#endif

#ifdef OUTADN
                SendXYZ(file, tt[0],tt[1],tt[2]);
#endif
                std::cout<<(float)(clock()-t)*1000/CLOCKS_PER_SEC<<"ms\n\n\n"<<std::flush;
#ifdef SOUND
                if(tt[2]>0)
                    if(tt[2]<1700)
                        mt.index=2;
                    else
                        mt.index=0;
                else
                    mt.index=1;
#endif
                cv::imshow("f",m);
                k=cv::waitKey(1);

            }
            else
            {
                std::cout<<"fail read image\n"<<std::flush;
#ifdef OUTADN
                SendXYZ(file, 0, 0,-1);
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
        fclose(file);
#endif

    }



    void test3(int w, int h, double scale,
               double markerLength=120, std::string port="/dev/ttyACM0")
    {
        model.ScaleModel(scale);

        cv::Mat m;

        cv::VideoCapture c(0);

        c.set(cv::CAP_PROP_FRAME_WIDTH, w);
        c.set(cv::CAP_PROP_FRAME_HEIGHT,h);
        c.set(cv::CAP_PROP_SETTINGS,1);

        cv::namedWindow("f",cv::WINDOW_NORMAL);

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
#endif
        int k=0;
        while(k!=27)
        {

            if(c.read(m))
            {
                clock_t t=clock();
#ifdef RECORDIMG
                cv::Mat m1;
                m.copyTo(m1);
#endif


                cv::Vec3d tt=MarkerPos(m,markerLength);

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
                std::cout<<(float)(clock()-t)*1000/CLOCKS_PER_SEC<<"ms\n\n\n"<<std::flush;
#ifdef SOUND
                if(tt[2]>0)
                    if(tt[2]<1700)
                        mt.index=2;
                    else
                        mt.index=0;
                else
                    mt.index=1;
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



    }




};

#endif // OCAMCV_HPP
