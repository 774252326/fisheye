#include "../ocamMeasure/findmarkerfisheye.hpp"

#include <thread>
#include <chrono>
#include <mutex>

class ToArduino
{
public:
    FILE *file;
    ToArduino(const char *port="/dev/ttyACM0")
    {
        file = fopen(port, "w");
    }

    ~ToArduino()
    {
        fclose(file);
    }

    void SendXYZ(float x, float y, float z)
    {
        fprintf(file, "%g,%g,%g,", x,y,z); //Writing to the file
        fflush(file);
    }
};


class AGVRed
{
public:
    FisheyeModel model;
    FindMarkerFisheye fmf;
    std::timed_mutex mtx;

    std::timed_mutex mtxDP;

    bool endflag;

    cv::Mat frame;
    cv::Mat frameDP;

public:
    AGVRed(std::string path)
        : model(path.c_str())
        , fmf(0)
    {

    }






    void detect(double scale, double markerLength=120)
    {
        model.Scale(scale);
        int64 t=cv::getTickCount();
        while(endflag)
        {

            if(mtx.try_lock_for(std::chrono::milliseconds(5)))
            {
                bool bprocess=frame.empty();
                if(!bprocess)
                {
                    frame.copyTo(frameDP);
                    frame.release();
                }
                mtx.unlock();


               if(!bprocess)
               {
                cv::Vec3d tt=fmf.Detect(2,frameDP,model,markerLength);
                fmf.DrawResult(frameDP,model,markerLength);
                std::cout<<tt<<'\n'<<std::flush;
                //                    cv::imshow("f",frame);
                //                    cv::waitKey(1);
                //                    std::this_thread::sleep_for(std::chrono::microseconds(1000));
                                    t=cv::getTickCount()-t;
                                    std::cout<<"time="<<((double)t)/cv::getTickFrequency()<<"\n"<<std::flush;
                                    t=cv::getTickCount();
               }
            }
        }
    }

    static void detect_thread(AGVRed *pdata, double scale)
    {
        pdata->detect(scale);
    }

    void capture(int w, int h)
    {
        cv::VideoCapture c(0);

        c.set(cv::CAP_PROP_FRAME_WIDTH, w);
        c.set(cv::CAP_PROP_FRAME_HEIGHT,h);
        c.set(cv::CAP_PROP_SETTINGS,1);

        while(endflag)
        {
                if(mtx.try_lock_for(std::chrono::milliseconds(5)))
                {
                    if(c.read(frame))
                    {
                    }
                    else
                    {
                        std::cout<<"fail read image\n"<<std::flush;
                    }
                    mtx.unlock();
                }
        }

        c.release();
    }

    static void capture_thread(AGVRed *pdata, int w, int h)
    {
        pdata->capture(w,h);
    }

    void display()
    {
         cv::namedWindow("f",cv::WINDOW_NORMAL);
         endflag=true;
         std::thread th1(capture_thread, this, 1280, 720);

         std::thread th2(detect_thread, this, 2.0/3);

         int k=0;
         while(k!=27)
         {
//             if(!frameDP.empty())
             {
//                 cv::imshow("f",frameDP);
             }
             k=cv::waitKey(1);
         }
         endflag=false;

         th1.join();
         th2.join();

    }

    void test3(int w, int h, double scale,
               double markerLength=120)
    {
        model.Scale(scale);

        cv::Mat m;

        cv::VideoCapture c(0);

        c.set(cv::CAP_PROP_FRAME_WIDTH, w);
        c.set(cv::CAP_PROP_FRAME_HEIGHT,h);
        c.set(cv::CAP_PROP_SETTINGS,1);

        cv::namedWindow("f",cv::WINDOW_NORMAL);

        cv::waitKey(3000);

        ToArduino ta("/dev/ttyACM0");

        int k=0;
        while(k!=27)
        {
            int64 t=cv::getTickCount();
            if(c.read(m))
            {
                cv::Vec3d tt=fmf.Detect(2,m,model,markerLength);
                ta.SendXYZ(tt[0],tt[1],tt[2]);
                fmf.DrawResult(m,model,markerLength);
                std::cout<<tt<<'\n'<<std::flush;
                cv::imshow("f",m);
                k=cv::waitKey(1);
            }
            else
            {
                std::cout<<"fail read image\n"<<std::flush;
                ta.SendXYZ(0,0,-1);
            }
            t=cv::getTickCount()-t;
            std::cout<<"time="<<((double)t)/cv::getTickFrequency()<<"\n"<<std::flush;
        }
        c.release();



    }
};



int main()
{
    AGVRed ar("2.1mm/calib_results1920x1080.txt");
    ar.test3(1280,720,2./3.);
//    ar.display();

    return 0;
}
