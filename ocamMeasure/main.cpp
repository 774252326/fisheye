
//#include "stereoocam.hpp"
#include "ocamcv.hpp"




int main(int argc, char **argv)
{
#if 1

    if(argc!=3)
    {
        OcamCV("2.1mm/calib_results1920x1080.txt").test3(1280,720,2.0/3);
        return 0;
    }
    double length;
    std::istringstream(argv[1])>>length;
    std::string port(argv[2]);
    std::cout<<length<<'\n'<<port<<'\n';
    OcamCV("2.1mm/calib_results1920x1080.txt").test3(1280,720,2.0/3,length,port);
    return 0;
#endif

    //        test();
    //Test();

#if 0
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
#endif


    return 0;
}
