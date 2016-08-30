#ifndef STEREOOCAM_HPP
#define STEREOOCAM_HPP

#include <opencv2/opencv.hpp>

#include "ocam_functions.h"

#include "Measure.hpp"

#include "ToArduino.hpp"

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
    struct ocam_model model[2];

    cv::Mat rt;

public:
    StereoOcam() {}

    int Load(const char *fp0, const char *fp1, std::string rtfp)
    {
        int flg0=get_ocam_model(&(model[0]), fp0);
        int flg1=get_ocam_model(&(model[1]), fp1);

        bool flgrt=Load(rt,rtfp);

        std::cout<<"rt="<<rt<<'\n'<<std::flush;

        if(flg0==0 && flg1==0 && flgrt)
            return 0;
        return -1;
    }


    static cv::Mat CeilMask(double hceil, double rejectRadius, ocam_model * pm)
    {
        cv::Mat mask(pm->height, pm->width, CV_8UC1, cv::Scalar(255));

        for(int i=0; i<mask.rows; i++)
        {
            for(int j=0;j<mask.cols;j++)
            {
                double pt2[2]={i,j};
                double pt3[3];
                cam2world(pt3,pt2,pm);

                if(pt3[0]<0)
                {
                    pt3[1]*=hceil/pt3[0];
                    pt3[2]*=hceil/pt3[0];

                    if(pt3[1]*pt3[1]+pt3[2]*pt3[2]<rejectRadius*rejectRadius)
                        mask.at<uchar>(i,j)=0;
                }

            }
        }

        return mask;
    }


    bool Triangulate(cv::Vec3d &p, cv::Point2f led0, cv::Point2f led1)
    {
        double point2D[2][2]={{led0.y,led0.x},{led1.y,led1.x}};
        double point3D[2][3];

        cam2world(point3D[0], point2D[0], &(model[0]));
        cam2world(point3D[1], point2D[1], &(model[1]));


        //            printf("\nworld2cam: pixel coordinates reprojected onto the image\n");
        //            printf("m_row= %2.4f, m_col=%2.4f\n", point2D[0][0], point2D[0][1]);
        //            printf("x= %2.4f, y=%2.4f, z=%2.4f\n", point3D[0][0], point3D[0][1], point3D[0][2]);
        //            printf("\nworld2cam: pixel coordinates reprojected onto the image\n");
        //            printf("m_row= %2.4f, m_col=%2.4f\n", point2D[1][1], point2D[1][1]);
        //            printf("x= %2.4f, y=%2.4f, z=%2.4f\n", point3D[1][0], point3D[1][1], point3D[1][2]);



        cv::Mat R;
        cv::Rodrigues(rt.rowRange(0,3),R);
        cv::Mat P1;
        cv::hconcat(R,rt.rowRange(3,6),P1);

        cv::Vec3d ray[2][2];
        ray[0][0]=cv::Vec3d(0,0,0);
        ray[0][1]=cv::Vec3d(point3D[0][1],point3D[0][0],-point3D[0][2]);
        ray[1][0]=cv::Vec3d(rt.rowRange(3,6));

        cv::Mat ray11=R*(cv::Mat_<double>(3,1)<<point3D[1][1],point3D[1][0],-point3D[1][2]);
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

        double radius=6000;
        double hceil=3500-1000;

        cv::Mat mask[2];
        mask[0]=CeilMask(hceil,radius,&(soc.model[0]));
        mask[1]=CeilMask(hceil+soc.rt.at<double>(4),radius,&(soc.model[0]));


        //        cv::imshow("mask0",mask[0]);
        //        cv::imshow("mask1",mask[1]);
        //        cv::waitKey();

        cv::Mat image[2];

        cv::VideoCapture c[2]={
            cv::VideoCapture(index1),
            cv::VideoCapture(index2)
        };

        c[0].set(CV_CAP_PROP_FRAME_WIDTH, 640);
        c[0].set(CV_CAP_PROP_FRAME_HEIGHT, 480);

        c[1].set(CV_CAP_PROP_FRAME_WIDTH, 640);
        c[1].set(CV_CAP_PROP_FRAME_HEIGHT, 480);


        int k=0;


        cv::namedWindow(winname1, cv::WINDOW_NORMAL);
        cv::namedWindow(winname2, cv::WINDOW_NORMAL);


        FILE *file = fopen("/dev/ttyACM0", "w");


        while(k!=27)
        {

            if(c[0].read(image[0]) && c[1].read(image[1]))
            {

                //             clock_t t=clock();

                cv::Point2f led1,led2;

                cv::Vec3d p;

                if(FindLed1m(image[0],led1,mask[0],thres1)
                        && FindLed1m(image[1],led2,mask[1],thres2)
                        && soc.Triangulate(p,led1,led2)
                        //&& cv::norm(p)>600
                        )

                    //                if(false && FindLed1(image[0],led1,thres1)
                    //                        && FindLed1(image[1],led2,thres2))
                {

                    cv::circle(image[0],led1,20,cv::Scalar(255,0,0),3);
                    cv::circle(image[1],led2,20,cv::Scalar(0,255,0),3);
                    std::cout<<"p="<<p<<'\n'<<std::flush;

                    SendXYZ(file, p[0],p[1],p[2]);


                }
                else
                {
                    std::cout<<"fail to find p\n"<<std::flush;

                    SendXYZ(file, 0, 0,-1);
                }

                //               std::cout<<(float)(clock()-t)*1000/CLOCKS_PER_SEC<<"ms\n"<<std::flush;

                cv::imshow(winname1,image[0]);
                cv::imshow(winname2,image[1]);
                k=cv::waitKey(1);
            }

        }

        fclose(file);
    }


};

#endif // STEREOOCAM_HPP
