#ifndef FINDMARKERFISHEYE_HPP
#define FINDMARKERFISHEYE_HPP
#include "findmarker.hpp"
#include "fisheyemodel.hpp"
#include <algorithm>
class FindMarkerFisheye : public FindMarker
{
public:
    FindMarkerFisheye(int dictionaryId=0)
        : FindMarker(dictionaryId)
    {

    }

    void MarkerPos(std::vector< int > &markerId, std::vector< cv::Vec3d > &rotateVector, std::vector< cv::Vec3d > &translateVector,
                   const cv::Mat &image, const FisheyeModel &camModel, float markerLength=3)
    {
        // detect markers and estimate pose
        cv::aruco::detectMarkers(image, dictionary, corners, markerId, detectorParams, rejected);

        rotateVector.clear();
        translateVector.clear();

        std::vector<cv::Point3d> pt3d(4,cv::Point3d(0,0,0));
        pt3d[1].x=markerLength;
        pt3d[2].x=markerLength;
        pt3d[2].y=markerLength;
        pt3d[3].y=markerLength;

        for(size_t i=0;i<markerId.size();)
        {
            if(corners[i].size()!=4)
            {
                markerId.erase(markerId.begin()+i);
                continue;
            }
            std::vector<cv::Point2d> ptsd(corners[i].size(),cv::Point2d());
            for(size_t j=0;j<corners[i].size();j++)
                ptsd[j]=corners[i][j];
            cv::Vec3d r,t;
            camModel.SolvePNPFisheye(ptsd,pt3d,r,t);
            rotateVector.push_back(r);
            translateVector.push_back(t);
            i++;
        }
    }


    cv::Vec3d Detect(int targetId,
                const cv::Mat &image, const FisheyeModel &camModel, float markerLength=3)
    {
        MarkerPos(ids, rvecs, tvecs, image, camModel, markerLength);
        std::vector<int>::iterator it = find (ids.begin(), ids.end(), targetId);
        if(it == ids.end())
        {
            return cv::Vec3d(0,0,-1);
        }
        size_t index=it-ids.begin();
        return tvecs[index];
    }


    void DrawResult(cv::Mat &imageColor, const FisheyeModel &camModel, float markerLength)
    {
        if(ids.empty())
            return;
        cv::aruco::drawDetectedMarkers(imageColor, corners, ids);

    }

};

#endif // FINDMARKERFISHEYE_HPP
