#ifndef TOARDUINO_HPP
#define TOARDUINO_HPP

#include <unistd.h>
#include <iostream>
#include <stdlib.h>
#include <time.h>

#include "rs232.hpp"

static int lost_count=6;
#if 0
static void UpdateSpeed(int &d, int &s,
                        float fa, float fb, float fc)
{
    int MAX_Speed = 70; //Range from 0 - 98
    int new_s, new_d;
    int accel_constant = 10;
    int tolar =11;
    int MAXrangeofA=330;
    int distance=600;
    int lost_count_constant=6;


    //==============================prevent over flow====================================
    if(fa > 32767) fa = 32767;
    else if(fa < -32768) fa = -32768;
    if(fb > 32767) fb = 32767;
    else if(fb < -32768) fb = -32768;
    if(fc > 32767) fc = 32767;
    else if(fc < -32768) fc = -1;
    //==============================convert to int ========================================
    int ina = fa;
    int inb = fb;
    int inc = fc;


    if(inc >= 1){ //if detect LED

        lost_count = lost_count_constant;

        new_s = ina;

        new_s =  new_s * MAX_Speed/MAXrangeofA;  //s will be add on the motor command fucntion

        //        if(new_s > 0)
        //        {Object_side = right;
        //        }else if(new_s < 0 ) Object_side = left;

        if(new_s < -MAX_Speed) new_s = (-MAX_Speed);
        else if(new_s > MAX_Speed) new_s = MAX_Speed;
        new_s = new_s * 0.4;


        new_d = (inc - distance)/10;
        if(new_d < 1) new_d = new_d * 2;
        if(new_d < -MAX_Speed) new_d = -MAX_Speed;
        else if(new_d > MAX_Speed) new_d = MAX_Speed;

        if((-tolar < new_s)&&(new_s < tolar)&&(-accel_constant <= s)&&(s <= accel_constant)&&(-tolar < new_d)&&(new_d < tolar)&&(-accel_constant <= d)&&(d <= accel_constant)) {
            s = 0;
            new_s = 0;
            new_d = 0;
            d = 0;
        }

    }
    else {                                                  // No blocks
        if(lost_count > -1)lost_count--;
        if(lost_count == -1 ){
            new_s = 0;
            new_d = 0;
            //            search = 0;
        }
    }


    if(s!=new_s) {
        if(s > new_s) {
            //                    s=s-(accel_constant*2);
            s=s-(accel_constant);
        }
        else {
            s=s+accel_constant;
        }

    }



    if(d!=new_d){
        if(d > new_d)
            //                      d=d-(2*accel_constant);
            d=d-(accel_constant);
        else d=d+accel_constant;
    }

    if(d<0)
        d=0;
    d=127+d;
    s=s+127;
}
#endif
static void UpdateSpeed(int &d, int &s,
                        float x, float y, float z,
                        float distance=600)
{
    int speeda=50;
    float upratioa=0.6;
    int speedb=12;
    float upratiob=0.2;
    float stoptangent=0.12;

    if(z>0)
     {
         if(z>distance)
         {
             d=d*upratioa+speeda*(1-upratioa);
         }
         else
         {
             d=d*upratioa;
         }

         if(x>z*stoptangent)
         {
             s=s*upratiob+speedb*(1-upratiob);
         }
         else
         {
             if(x>-z*stoptangent)
                 s=0;
             else
                 s=s*upratiob-speedb*(1-upratiob);
         }
     }
     else
     {
         d=d*upratioa;
         s=s*upratiob;
     }

    d=127+d;
    s=s+127;
}


static void SendDS(FILE *file, char d, char s)
{
    fprintf(file, "%c%c", d, s); //Writing to the file
    fflush(file);
}


static void SendXYZ(FILE *file, float x, float y, float z)
{


    fprintf(file, "%g,%g,%g,", x,y,z); //Writing to the file
    fflush(file);
}

template<typename T>
void SendValue(int comport_number, T value)
{
    RS232::SendBuf(comport_number, (unsigned char *)(&value), sizeof(value));
}



template<typename T>
bool ReceiveValue(int comport_number, T &value, int timeout=100)
{
    unsigned char *pr=(unsigned char *)(&value);
    int nres=sizeof(value);
    int k;
    for(k=0;k<timeout;k++)
    {
        usleep(1000);
        int n = RS232::PollComport(comport_number, pr, nres);
        if(n>0)
        {
            //            printf("received %i bytes: %d\n", n, value);
            //            perror("try");
            nres-=n;
            pr+=n;
            if(nres<=0)
            {
                return true;
            }
        }
    }
    return false;
}

template<typename T>
bool SendValue(int comport_number, T value, int retry)
{
    SendValue(comport_number,value);

    for(int i=0;i<retry;i++)
    {
        T res;
        bool flg=ReceiveValue(comport_number,res);
        if(flg && res==value)
            return true;
    }
    return false;
}

static void test()
{

    srand(time(NULL));


    FILE *file = fopen("/dev/ttyACM0", "rw");

    char buf[256];

    for(int i=0;i<200;i++)
    {
        int ms=rand()%100+50;

        char msc=ms;
        fprintf(file, "%c%c", msc,msc); //Writing to the file
        fflush(file);

        usleep(ms*1000);

        fscanf(file,"%s",buf);

        std::cout<<i<<"\t"<<msc<<"\t"<<ms<<"ms\n"
                <<"buf="<<buf<<"\n"<<std::flush;


    }
    fclose(file);
}

static void test2()
{
    int i=463,
            cport_nr=24,        /* /dev/ttyS0 (COM1 on windows) */
            bdrate=19200;       /* 9600 baud */

    char mode[]={'8','N','1',0};


    if(RS232::OpenComport(cport_nr, bdrate, mode))
    {
        printf("Can not open comport\n");

        return;
    }

    int j=0;
    for(;j<100;j++)
    {

        printf("sent: %d\n", i);

        bool flg=SendValue(cport_nr,(short)i,60);

        if(flg)
        {
            i++;
            printf("yes\n");
        }
        else
            printf("no\n");

        //        if(flg)
        //            perror("yes");
        //        else
        //            perror("no");


    }

    RS232::CloseComport(cport_nr);
}


#endif // TOARDUINO_HPP
