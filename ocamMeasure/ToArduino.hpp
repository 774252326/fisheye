#ifndef TOARDUINO_HPP
#define TOARDUINO_HPP

#include <unistd.h>
#include <iostream>
#include <stdlib.h>
#include <time.h>


static int lost_count=6;

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



static void test()
{

    srand(time(NULL));


    FILE *file = fopen("/dev/ttyACM1", "w");

    char buf[256];

    for(int i=0;i<200;i++)
    {
        int ms=rand()%100+50;

        SendXYZ(file,1,1,1);

//        char msc=ms;
//        fprintf(file, "%c%c", msc,msc); //Writing to the file
//        fflush(file);

        usleep(ms*1000);

//        fscanf(file,"%s",buf);

//        std::cout<<i<<"\t"<<msc<<"\t"<<ms<<"ms\n"
//                <<"buf="<<buf<<"\n"<<std::flush;


    }
    fclose(file);
}



#endif // TOARDUINO_HPP
