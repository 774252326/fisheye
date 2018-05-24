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
        , objflag(false)
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


    static bool outsth1(FILE* &file, char *str)
    {
        int r1=fputs(str, file); //Writing to the file
        if(ferror(file) || r1 < 0)
        {
            printf("fprintf fail\n");
            return false;
        }
        int r2=fflush(file);
        if(ferror(file))
        {
            printf("fflush fail\n");
            return false;
        }
        return true;
    }

    static void outsth(FILE* &file, char *str, unsigned int maxIndex=2)
    {
        unsigned int i=0;

        while(file==NULL || !outsth1(file, str))
        {
            char portName[32];
            sprintf(portName, "/dev/ttyACM%u", i);
            printf("ST disconnected, try open %s\n", portName);
            file=fopen(portName, "w");
            usleep(3000);
            i++;
            if(i>maxIndex) i=0;
        }
    }


    static void SendXYZ2(FILE* &file, float x, float y, float z)
    {
        char buf[64];
        sprintf(buf, "%g,%g,%g,", x,y,z); //Writing to the file
        outsth(file, buf);
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

            bool got=false;

            double ppp[3]={0,0,-1};

            ps->acquire();
            if(dirtyflag)
            {
                if(!objflag)
                {
                  ppp[0]=pos[0];
                  ppp[1]=pos[1];
                  ppp[2]=pos[2];
                }                
                got=true;
                dirtyflag=false;
            }
            ps->release();

            if(got)
            {
                SendXYZ2(file, ppp[0], ppp[1], ppp[2]);
            }

            msleep(1);
        }

        fclose(file);

    }

};

#endif // CONTROLTHREAD_HPP
