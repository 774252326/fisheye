#include "ocamcv.hpp"




int main(int argc, char **argv)
{
#if 1

    if(argc!=4)
    {
        OcamCV("2.1mm/calib_results1920x1080.txt").test3(1280,720,2.0/3);
        return 0;
    }
    std::string campath(argv[1]);
    double length;
    std::istringstream(argv[2])>>length;
    std::string port(argv[3]);
    std::cout<<campath<<"\n"<<length<<'\n'<<port<<'\n';
    OcamCV(campath).test3(1280,720,2.0/3,length,port);
    return 0;
#endif



    return 0;
}
