#ifndef STEREOOCAM_HPP
#define STEREOOCAM_HPP

#include <opencv2/opencv.hpp>

#include "fisheyemodel.hpp"

#include "Measure.hpp"

#include "ToArduino.hpp"

#define VSN
//#define CEILMASK

class StereoOcam
{
private:
    static bool Load(cv::Mat &m, std::string fp)
    {
        cv::FileStorage fs(fp, CV_STORAGE_READ);
        if (fs.isOpened())
        {
            fs["m"] >> m;
            fs.release();
            return true;
        }
        std::cout << "Failed to open file " << fp << '\n';
        return false;
    }
public:
    FisheyeModel model[2];

    cv::Mat rt;

public:
    StereoOcam() {}

    int Load(const char *fp0, const char *fp1, std::string rtfp)
    {
        int flg0=model[0].Load(fp0);
        int flg1=model[1].Load(fp1);

        bool flgrt=Load(rt,rtfp);

        std::cout<<"rt="<<rt<<'\n'<<std::flush;

        if(flg0==0 && flg1==0 && flgrt)
            return 0;
        return -1;
    }


    static cv::Mat CeilMask(double hceil, double rejectRadius, FisheyeModel * pm)
    {
        cv::Mat mask(pm->height, pm->width, CV_8UC1, cv::Scalar(255));

        for(int i=0; i<mask.rows; i++)
        {
            for(int j=0;j<mask.cols;j++)
            {
                pm->cam2world(pt3,pt2);

                cv::Point3d pt3=pm->Cam2WorldUnit(cv::Point2d(j,i));

                if(pt3.y<0)
                {
                    pt3.x*=hceil/pt3.y;
                    pt3.z*=hceil/pt3.y;

                    if(pt3.x*pt3.x+pt3.z*pt3.z<rejectRadius*rejectRadius)
                        mask.at<uchar>(i,j)=0;
                }

            }
        }

        return mask;
    }





    bool Triangulate(cv::Vec3d &p, cv::Point2f led0, cv::Point2f led1)
    {
        cv::Point3d point0=model[0].Cam2WorldUnit(cv::Point2d(led0));
        cv::Point3d point1=model[1].Cam2WorldUnit(cv::Point2d(led1));

        cv::Mat R;
        cv::Rodrigues(rt.rowRange(0,3),R);
        cv::Mat P1;
        cv::hconcat(R,rt.rowRange(3,6),P1);

        cv::Vec3d ray[2][2];
        ray[0][0]=cv::Vec3d(0,0,0);
        ray[0][1]=cv::Vec3d(point0.x, point0.y, point0.z);
        ray[1][0]=cv::Vec3d(rt.rowRange(3,6));

        cv::Mat ray11=R*(cv::Mat_<double>(3,1)<<point1.x, point1.y, point1.z);
        ray[1][1]=cv::Vec3d(ray11);

        //        std::cout<<"ray="<<ray[0][0]<<'\n'
        //                           <<ray[0][1]<<'\n'
        //                                        <<ray[1][0]<<'\n'
        //                                                     <<ray[1][1]<<'\n'
        //                                                               <<std::flush;

        cv::Matx22d A(ray[0][1].ddot(ray[0][1]), -ray[0][1].ddot(ray[1][1]),
                ray[0][1].ddot(ray[1][1]), -ray[1][1].ddot(ray[1][1]));

        if(A.val[0]*A.val[3]==A.val[2]*A.val[1])
        {
            std::cout<<"parallel ray\n"<<std::flush;
            return false;
        }

        cv::Vec3d db=ray[1][0]-ray[0][0];

        cv::Vec2d b(db.ddot(ray[0][1]), db.ddot(ray[1][1]));

        cv::Vec2d k=A.inv()*b;

        if(k[0]<0 || k[1]<0)
        {
            std::cout<<"behind ray\n";
            return false;
        }

        p=(ray[0][0]+k[0]*ray[0][1]+ray[1][0]+k[1]*ray[1][1])/2;

        return true;
    }



    static void DualCamTest3(uchar thres1=160, uchar thres2=160, int index1=0, int index2=1, std::string winname1="usb1", std::string winname2="usb2")
    {


        StereoOcam soc;
#if 1
        std::string folder="/home/pi/fisheye/ocamMeasure/";

        std::string fn0=folder+"usb3/calib_results.txt";
        std::string fn1=folder+"usb4/calib_results.txt";
        std::string fnrt=folder+"dual/rt1.yml";
        soc.Load(fn0.c_str(),fn1.c_str(),fnrt);
#else
        soc.Load("C:\\Users\\jhanbin\\Desktop\\pro\\fisheye\\Scaramuzza_OCamCalib_v3.0_win\\usb4\\calib_results.txt",
                 "C:\\Users\\jhanbin\\Desktop\\pro\\fisheye\\Scaramuzza_OCamCalib_v3.0_win\\usb3\\calib_results.txt",
                 "C:\\Users\\jhanbin\\Desktop\\piS\\untitled\\dual\\rt1.yml");
#endif


#ifdef CEILMASK
        double radius=6000;
        double hceil=3500-1000;

        cv::Mat mask[2];
        mask[0]=CeilMask(hceil,radius,&(soc.model[0]));
        mask[1]=CeilMask(hceil+soc.rt.at<double>(4),radius,&(soc.model[0]));


        cv::imshow("mask0",mask[0]);
        cv::imshow("mask1",mask[1]);
        cv::imwrite("mask0.png",mask[0]);
        cv::imwrite("mask1.png",mask[1]);
        cv::waitKey();
        cv::destroyWindow("mask0");
        cv::destroyWindow("mask1");
#endif

        cv::Mat image[2];

        cv::VideoCapture c[2]={
            cv::VideoCapture(index1),
            cv::VideoCapture(index2)
        };

        c[0].set(CV_CAP_PROP_FRAME_WIDTH, 640);
        c[0].set(CV_CAP_PROP_FRAME_HEIGHT, 480);

        //        c[0].set(CV_CAP_PROP_EXPOSURE,-13);
        //        c[0].set(CV_CAP_PROP_GAIN,0);

        c[1].set(CV_CAP_PROP_FRAME_WIDTH, 640);
        c[1].set(CV_CAP_PROP_FRAME_HEIGHT, 480);

        //        c[1].set(CV_CAP_PROP_EXPOSURE,-13);
        //        c[1].set(CV_CAP_PROP_GAIN,0);

        int k=0;


        cv::namedWindow(winname1, cv::WINDOW_NORMAL);
        cv::namedWindow(winname2, cv::WINDOW_NORMAL);

#ifdef VSN
        FILE *file = fopen("/dev/ttyACM0", "w");
        //        FILE *file = fopen("/dev/ttyUSB0", "w");
#else
        int cport_nr=24,        /* /dev/ttyS0 (COM1 on windows) */
                bdrate=19200;       /* 9600 baud */
        char mode[]={'8','N','1',0};

        if(RS232::OpenComport(cport_nr, bdrate, mode))
        {
            printf("Can not open comport\n");
            return;
        }

        int ds[2]={0,0};

#endif

        while(k!=27)
        {

            if(c[0].read(image[0]) && c[1].read(image[1]))
            {

                //                             clock_t t=clock();

                cv::Point2f led1,led2;

                cv::Vec3d p;
#ifdef CEILMASK
                if(FindLed1m(image[0],led1,mask[0],thres1)
                        && FindLed1m(image[1],led2,mask[1],thres2)
                        && soc.Triangulate(p,led1,led2)
                        //&& cv::norm(p)>600
                        )
#else
                if(FindLed1(image[0],led1,thres1)
                        && FindLed1(image[1],led2,thres2)
                        && soc.Triangulate(p,led1,led2))
#endif
                {

                    cv::circle(image[0],led1,20,cv::Scalar(255,0,0),3);
                    cv::circle(image[1],led2,20,cv::Scalar(0,255,0),3);
                    std::cout<<"p="<<p<<'\n'<<std::flush;
#ifdef VSN
                    SendXYZ(file, p[0],p[1],p[2]);
#else
                    UpdateSpeed(ds[0],ds[1],p[0],p[1],p[2],600);
                    ds[0]=0;
                    short speeddata=(ds[0]<<8)|ds[1];

                    SendValue(cport_nr,speeddata,1);


#endif

                }
                else
                {
                    std::cout<<"fail to find p\n"<<std::flush;
#ifdef VSN
                    SendXYZ(file, 0, 0,-1);
#else
                    UpdateSpeed(ds[0],ds[1],0,0,-1,600);
                    ds[0]=0;
                    short speeddata=(ds[0]<<8)|ds[1];

                    SendValue(cport_nr,speeddata,1);

#endif
                }

                //                               std::cout<<(float)(clock()-t)*1000/CLOCKS_PER_SEC<<"ms\n"<<std::flush;
#ifdef CEILMASK
                image[0].setTo(0,255-mask[0]);
                image[1].setTo(0,255-mask[1]);
#endif
                cv::imshow(winname1,image[0]);
                cv::imshow(winname2,image[1]);
                k=cv::waitKey(1);
            }

        }

#ifdef VSN
        fclose(file);
#else
        RS232::CloseComport(cport_nr);
#endif
    }


};

#endif // STEREOOCAM_HPP
