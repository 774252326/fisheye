
#include "stereoocam.hpp"


int main(int argc, char **argv)
{
    if(argc!=3)
    {
         StereoOcam::DualCamTest3();
         return 0;
    }


    double th1;
    double th2;
    std::istringstream(argv[1])>>th1;
    std::istringstream(argv[2])>>th2;
    StereoOcam::DualCamTest3(th1,th2);



//    test();

    return 0;
}
