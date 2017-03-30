#ifndef FINDMARKER_HPP
#define FINDMARKER_HPP

#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>


class FindMarker
{
public:
    cv::Ptr<cv::aruco::DetectorParameters> detectorParams;
    cv::Ptr<cv::aruco::Dictionary> dictionary;

    std::vector< int > ids;
    std::vector< std::vector< cv::Point2f > > corners;
    std::vector< std::vector< cv::Point2f > > rejected;
    std::vector< cv::Vec3d > rvecs;
    std::vector< cv::Vec3d > tvecs;

    double distance;

    std::string res;

public:
    FindMarker(int dictionaryId=11)
    {
        detectorParams = cv::aruco::DetectorParameters::create();
        detectorParams->doCornerRefinement = true; // do corner refinement in markers
        dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::PREDEFINED_DICTIONARY_NAME(dictionaryId));
    }

    void MarkerPos(std::vector< int > &markerId, std::vector< cv::Vec3d > &rotateVector, std::vector< cv::Vec3d > &translateVector,
                   const cv::Mat &image, const cv::Mat &camMatrix, const cv::Mat &distCoeffs, float markerLength=3)
    {
        // detect markers and estimate pose
        cv::aruco::detectMarkers(image, dictionary, corners, markerId, detectorParams, rejected);
        if(markerId.empty())
        {
            rotateVector.clear();
            translateVector.clear();
            return;
        }
        cv::aruco::estimatePoseSingleMarkers(corners, markerLength, camMatrix, distCoeffs, rotateVector, translateVector);
        cv::Vec3d rv3, tv3;
        for(size_t i=0;i<rotateVector.size();i++)
        {
            cv::composeRT(cv::Vec3d(CV_PI,0,0),cv::Vec3d(0,0,0),rvecs[i],tvecs[i],rv3,tv3);
            rvecs[i]=rv3;
            tvecs[i]=tv3;
        }
    }

    bool Detect(double &angle, double &centerX, double &centerY, int &markerId,
                const cv::Mat &image,
                const cv::Mat &camMatrix, const cv::Mat &distCoeffs, float markerLength=3)
    {
        MarkerPos(ids, rvecs, tvecs, image, camMatrix, distCoeffs, markerLength);

        if(ids.empty())
        {
            return false;
        }

        cv::Matx33d rm33;
        cv::Rodrigues(rvecs[0],rm33);

        double angle2=atan2(rm33(1,0),rm33(0,0));

        angle=angle2;

        centerX=tvecs[0][0];
        centerY=tvecs[0][1];
        markerId=ids[0];

        distance=tvecs[0][2];

        std::ostringstream oss;
        oss<<centerX<<"  "<<centerY<<"  "<<angle;
        //        oss<<angle<<"  "<<angle2;

        res=oss.str();

        return true;
    }

    void DrawResult(cv::Mat &imageColor)
    {
        if(ids.empty())
            return;
        cv::aruco::drawDetectedMarkers(imageColor, corners, ids);
    }

    void DrawResult(cv::Mat &imageColor, const cv::Mat &camMatrix, const cv::Mat &distCoeffs, float markerLength)
    {
        if(ids.empty())
            return;
        cv::aruco::drawDetectedMarkers(imageColor, corners, ids);
        cv::aruco::drawAxis(imageColor, camMatrix, distCoeffs, rvecs[0], tvecs[0], markerLength * 0.5f);

        cv::putText(imageColor,res,cv::Point(0,imageColor.rows-1),cv::FONT_HERSHEY_SIMPLEX,0.6,cv::Scalar(0,0,255),2);
    }

};

#endif // FINDMARKER_HPP
