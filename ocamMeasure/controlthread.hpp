#ifndef CONTROLTHREAD_HPP
#define CONTROLTHREAD_HPP
#include <QtCore>
#include <QSemaphore>
#include <string>
#include <opencv2/opencv.hpp>
#include "ToArduino.hpp"

class ControlThread : public QThread
{
public:
    std::string port;
    double pos[3];
    bool dirtyflag;
    bool objflag;
    bool goflag;
    QSemaphore *ps;

    ControlThread(QSemaphore *s0)
        : ps(s0)
        , goflag(false)
    {

    }

    void SetPos(cv::Vec3d pt)
    {
        ps->acquire();
        pos[0]=pt[0];
        pos[1]=pt[1];
        pos[2]=pt[2];
        dirtyflag=true;
        ps->release();
    }

    void SetObj(bool flag)
    {
        ps->acquire();
        objflag=flag;
        dirtyflag=true;
        ps->release();
    }

    void run()
    {
        goflag=true;

        FILE *file = fopen(port.c_str(), "w");

        while(goflag)
        {
//            if(objflag)
//            {
//                SendXYZ(file, 0, 0,-1);
//                msleep(1);
//                continue;
//            }


            ps->acquire();
            if(dirtyflag)
            {
                if(objflag)
                {
                    SendXYZ(file, 0, 0,-1);
                }
                else
                {
                SendXYZ(file, pos[0],pos[1],pos[2]);
                }
                dirtyflag=false;
            }
            ps->release();

            msleep(1);
        }

        fclose(file);

    }

};

#endif // CONTROLTHREAD_HPP
