#ifndef FISHEYEMODEL_HPP
#define FISHEYEMODEL_HPP

#include <opencv2/opencv.hpp>

#define USEOCAMMODEL

#ifdef USEOCAMMODEL
#include "ocam_model.hpp"
#else
#endif

class FisheyeModel
{

    static void ComputeTransform(cv::Vec3d &xyz, cv::Vec3d &rxyz, const cv::Vec3d ptFrom[4], const cv::Vec3d ptTo[4])
    {
        cv::Vec3d ctFrom = (ptFrom[0] + ptFrom[1] + ptFrom[2] + ptFrom[3]) / 4;
        cv::Vec3d ctTo = (ptTo[0] + ptTo[1] + ptTo[2] + ptTo[3]) / 4;
        cv::Matx33d H = (ptFrom[0] - ctFrom)*((ptTo[0] - ctTo).t());
        H += (ptFrom[1] - ctFrom)*((ptTo[1] - ctTo).t());
        H += (ptFrom[2] - ctFrom)*((ptTo[2] - ctTo).t());
        H += (ptFrom[3] - ctFrom)*((ptTo[3] - ctTo).t());
        cv::Mat U, S, Vt;
        cv::SVDecomp(H, S, U, Vt);
        cv::Mat R0 = (U*Vt).t();
        cv::Matx33d R((double*)R0.ptr());
        cv::Rodrigues(R, rxyz);
        xyz = ctTo - R*ctFrom;
    }

public:
#ifdef USEOCAMMODEL
    ocam_model model;
#else
    //model used: R=scale*theta
    cv::Point2d center;
    double scale;
#endif
    FisheyeModel(const char *filename=NULL)
    {
        Load(filename);
    }

    int Load(const char *filename=NULL)
    {
#ifdef USEOCAMMODEL
   return model.Load(filename);
#else
return 0;
#endif
    }

    void Scale(double ratio)
    {
#ifdef USEOCAMMODEL
        model.ScaleModel(ratio);
#else
        scale*=ratio;
        center.x*=ratio;
        center.y*=ratio;
#endif
    }

    static cv::Mat ComputePureRotateM(const cv::Vec3d ptFrom[3], const cv::Vec3d ptTo[3])
    {
        cv::Vec3d ctFrom = (ptFrom[0] + ptFrom[1] + ptFrom[2]) / 4;
        cv::Vec3d ctTo = (ptTo[0] + ptTo[1] + ptTo[2]) / 4;
        cv::Matx33d H = (ptFrom[0] - ctFrom)*((ptTo[0] - ctTo).t());
        H += (ptFrom[1] - ctFrom)*((ptTo[1] - ctTo).t());
        H += (ptFrom[2] - ctFrom)*((ptTo[2] - ctTo).t());
        H += (ctFrom)*(ctTo.t());
        cv::Mat U, S, Vt;
        cv::SVDecomp(H, S, U, Vt);
        cv::Mat R0 = (U*Vt).t();
        return R0;
    }

    cv::Point3d Cam2WorldUnit(cv::Point2d pixel) const
    {
#ifdef USEOCAMMODEL
        double point2D[2]={pixel.y,pixel.x};
        double point3D[3];
        model.cam2world(point3D, point2D);
        return cv::Point3d(point3D[1],point3D[0],-point3D[2]);
#else
        cv::Point2d dfpixel = pixel - center;
        double beta = atan2(dfpixel.y, dfpixel.x);
        double alpha = sqrt(dfpixel.ddot(dfpixel)) / scale;
        cv::Point3d xyz;
        xyz.z = cos(alpha);
        xyz.x = sin(alpha)*cos(beta);
        xyz.y = sin(alpha)*sin(beta);
        return xyz;
#endif
    }

    void SolvePNPFisheye(const std::vector<cv::Point2d> &pixel,
        const std::vector<cv::Point3d> &point,
        cv::Vec3d &Rv,
        cv::Vec3d &Tv) const
    {
        std::vector<cv::Point3d> pixel3dunit(pixel.size(), cv::Point3d());

        cv::Vec3d newz(0, 0, 0);

        for (size_t i = 0; i<pixel.size(); i++)
        {
            pixel3dunit[i] = Cam2WorldUnit(pixel[i]);
            newz = newz + cv::Vec3d(pixel3dunit[i]);
        }

        double Anewz = cv::norm(newz);

        newz = newz / Anewz;

        cv::Vec3d newx = cv::Vec3d(0, 1, 0).cross(newz);

        double Anewx = cv::norm(newx);
        newx = newx / Anewx;

        cv::Vec3d newy = newz.cross(newx);


        cv::Vec3d p1[3] = {
            cv::Vec3d(1,0,0),
            cv::Vec3d(0,1,0),
            cv::Vec3d(0,0,1)
        };

        cv::Vec3d p0[3] = {
            newx,
            newy,
            newz
        };

        cv::Mat R = ComputePureRotateM(p0, p1);

        cv::Matx33d R33((double*)R.ptr());

        //        std::cout<<"R33="<<R33<<"\n"<<std::flush;

        for (size_t i = 0; i<pixel3dunit.size(); i++)
        {
            pixel3dunit[i] = R33*pixel3dunit[i];
        }


        cv::Mat K = cv::Mat::eye(3, 3, R.type());
        cv::Mat D(5, 1, R.type(), cv::Scalar(0));

        std::vector<cv::Point2d> pixelw(pixel.size(), cv::Point2d());

        cv::projectPoints(pixel3dunit, cv::Vec3d(0, 0, 0), cv::Vec3d(0, 0, 0), K, D, pixelw);

        cv::Mat rv;
        cv::Mat tv;

        cv::solvePnP(point, pixelw, K, D, rv, tv);

        cv::Mat rvf;
        cv::Mat tvf;

        cv::Mat rv2;
        cv::Rodrigues(R.inv(), rv2);
        cv::Mat tv2(3, 1, rv2.type(), cv::Scalar(0));

        cv::composeRT(rv, tv, rv2, tv2, rvf, tvf);

        Rv = cv::Vec3d((double*)rvf.ptr());
        Tv = cv::Vec3d((double*)tvf.ptr());

//        std::cout << "RT=" << Rv << Tv << "\n" << std::flush;
    }

};


#endif // FISHEYEMODEL_HPP
