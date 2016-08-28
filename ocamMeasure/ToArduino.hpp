#ifndef TOARDUINO_HPP
#define TOARDUINO_HPP

#include <unistd.h>

static void SendXYZ(FILE *file, float x, float y, float z)
{
    fprintf(file, "%g , %g , %g, ", x,y,z); //Writing to the file
    fflush(file);
}

static void test()
{
    FILE *file = fopen("/dev/ttyACM1", "w");
    for(int i=0;i<1000;i++)
    {
        SendXYZ(file, i/10.,456,789);
        usleep(500000);
//        printf("%d\n",i);
    }
    fclose(file);
}


#endif // TOARDUINO_HPP
