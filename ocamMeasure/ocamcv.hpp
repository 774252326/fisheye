#ifndef OCAMCV_HPP
#define OCAMCV_HPP
#include <opencv2/opencv.hpp>
#include "ocam_functions.h"

class OcamCV
{
public:
    ocam_model model;
public:
    OcamCV(std::string path)
    {
        LoadModel(path);
    }

    int LoadModel(std::string path)
    {
        return get_ocam_model(&model, path.c_str());
    }

    cv::Point3d Cam2World(cv::Point2d pixel)
    {
        double point2D[2]={pixel.y,pixel.x};
        double point3D[3];
        cam2world(point3D, point2D, &model);
        return cv::Point3d(point3D[1],point3D[0],-point3D[2]);
    }

    void SolvePNPFisheye(const std::vector<cv::Point2d> &pixel,
                                const std::vector<cv::Point3d> &point,
                                cv::Vec3d &Rv,
                                cv::Vec3d &Tv)
    {

    }
};

#endif // OCAMCV_HPP
