#ifndef TOARDUINO_HPP
#define TOARDUINO_HPP

#include <unistd.h>
#include <iostream>
#include <stdlib.h>
#include <time.h>


static int lost_count=6;

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

static void test()
{

    srand(time(NULL));


    FILE *file = fopen("/dev/ttyACM0", "w");
    for(int i=0;i<200;i++)
    {
                SendXYZ(file, i/1e1,i/1e2,i<
                        100?7.89:8);


        int ms=rand()%100+50;

        char msc=ms;
//        fprintf(file, "%c%c", msc,msc); //Writing to the file
//        fflush(file);

        usleep(ms*1000);
        std::cout<<i<<"\t"<<msc<<"\t"<<ms<<"ms\n"<<std::flush;
        //        printf("%d\n",i);
    }
    fclose(file);
}


#endif // TOARDUINO_HPP
