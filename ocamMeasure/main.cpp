
#include "stereoocam.hpp"

template <typename T>
T PolyValue(const T *coef, int nCoef, T x)
{
    int i=nCoef-1;
    T y=coef[i];
    while(i>0)
    {
        y*=x;
        i--;
        y+=coef[i];
    }
    return y;
}







int main(int argc, char **argv)
{

//    struct ocam_model model;
//    int flg0=get_ocam_model(&(model), "C:\\Users\\jhanbin\\Desktop\\piS\\untitled\\usb4\\calib_results.txt");

//    double p=model.width/2;
//    double fp=PolyValue(model.pol,model.length_pol,p);
//    double theta=atan2(p,fp);

//    std::cout<<"theta="<<theta<<'\n'<<std::flush;

//    double D=300;
//    double markerl=theta/640*D*6*3;

// std::cout<<"markerl="<<markerl<<'\n'<<std::flush;

//    return 1;

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
