#ifndef CAMERA_HPP
#define CAMERA_HPP


#include <iostream>
#include <opencv2/opencv.hpp>

class CAMERA
{
public:
    static void TestCV(std::string fp="/home/pi/opencv-3.1.0/samples/data/baboon.jpg", int wait=10000)
    {
        cv::Mat m=cv::imread(fp);
        cv::imshow(fp,m);
        cv::waitKey(wait);
        cv::destroyWindow(fp);
    }

    static void SaveImg(std::string folder, const cv::Mat &image, int &index)
    {
        char buf[50];
        sprintf(buf,"%03d.png",index);
        if(cv::imwrite(folder+std::string(buf),image))
        {
            std::cout<<"save "<<buf<<'\n'<<std::flush;
            index++;
        }
        else
        {
           std::cout<<"fail to save "<<buf<<'\n'<<std::flush;
        }
    }

    static void TestCam(int id=0, std::string folder="/home/pi/untitled/", int countdown=0, std::string winname="frame")
    {
        cv::VideoCapture c(id);

        c.set(CV_CAP_PROP_SETTINGS,1);

        c.set(CV_CAP_PROP_FRAME_WIDTH,1920);

        c.set(CV_CAP_PROP_FRAME_HEIGHT,1080);

        cv::Mat m;
        int index=0;
        int key=0;
        int count=countdown;
        cv::namedWindow(winname, cv::WINDOW_NORMAL);
        while(key!=27)
        {
            if(c.read(m))
            {
                cv::imshow(winname,m);
            }
            key=cv::waitKey(1);

            if(countdown>0)
            {
                std::cout<<count<<'\n'<<std::flush;
                if(count==0)
                {
                    SaveImg(folder,m,index);
                    count=countdown;
                    continue;
                }
                count--;
                continue;
            }

            if(key=='c')
            {
                SaveImg(folder,m,index);
            }

        }
    }



};

#endif // CAMERA_HPP
