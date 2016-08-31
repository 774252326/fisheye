#ifndef MEASURE_HPP
#define MEASURE_HPP

#include <iostream>
#include <opencv2/opencv.hpp>

#include "ConnectComponent.hpp"
#include <algorithm>

static void SortLed(std::vector<cv::Point2f> &led)
{
    std::vector<float> x(led.size()),y(led.size());

    for(size_t i=0;i<led.size();i++)
    {
        x[i]=led[i].x;
        y[i]=led[i].y;
    }

    size_t xmin[2], ymin[2];

    xmin[0]=std::min_element(x.begin(),x.end())-x.begin();
    x.erase(x.begin()+xmin[0]);
    xmin[1]=std::min_element(x.begin(),x.end())-x.begin();
    if(xmin[1]>=xmin[0])
        xmin[1]++;
    else
    {
        size_t tmp=xmin[0];
        xmin[0]=xmin[1];
        xmin[1]=tmp;
    }


    std::cout<<"xmin\n"<<xmin[0]<<'\t'<<xmin[1]<<'\t'<<std::flush;

    ymin[0]=std::min_element(y.begin(),y.end())-y.begin();
    y.erase(y.begin()+ymin[0]);
    ymin[1]=std::min_element(y.begin(),y.end())-y.begin();
    if(ymin[1]>=ymin[0])
        ymin[1]++;
    {
        size_t tmp=ymin[0];
        ymin[0]=ymin[1];
        ymin[1]=tmp;
    }

    std::cout<<"ymin\n"<<ymin[0]<<'\t'<<ymin[1]<<'\t'<<std::flush;

    size_t fidx[4];

    std::set_intersection (xmin, xmin+2, ymin, ymin+2, fidx+0);

    std::set_difference (xmin, xmin+2, ymin, ymin+2, fidx+1);

    std::set_difference (ymin, ymin+2, xmin, xmin+2, fidx+2);

    size_t oidx[]={0,1,2,3};


    std::set_difference (oidx, oidx+4, fidx, fidx+3, fidx+3);

    std::cout<<"fidx\n"<<fidx[0]<<'\t'<<fidx[1]<<'\t'<<fidx[2]<<'\t'<<fidx[3]<<'\t'<<std::flush;


    led.push_back(led[fidx[0]]);
    led.push_back(led[fidx[1]]);
    led.push_back(led[fidx[2]]);
    led.push_back(led[fidx[3]]);
    led.erase(led.begin(),led.begin()+4);


}


static void SortLed2(std::vector<cv::Point2f> &led)
{
    size_t idx[4];

    float d[4];
    for(size_t i=0;i<4;i++)
        d[i]=led[i].x+led[i].y;

    idx[0]=std::min_element(d,d+4)-d;
    idx[3]=std::max_element(d,d+4)-d;


    for(size_t i=0;i<4;i++)
        d[i]=led[i].x-led[i].y;

    idx[2]=std::min_element(d,d+4)-d;
    idx[1]=std::max_element(d,d+4)-d;

    led.push_back(led[idx[0]]);
    led.push_back(led[idx[1]]);
    led.push_back(led[idx[2]]);
    led.push_back(led[idx[3]]);
    led.erase(led.begin(),led.begin()+4);

}


static bool FindLed(std::vector<cv::Point2f> &pxl, const cv::Mat &m, uchar thres=200)
{
    cv::Mat bw;
    cv::threshold(m,bw,thres,255,cv::THRESH_BINARY);

    cv::namedWindow("bw", cv::WINDOW_NORMAL);
    cv::imshow("bw",bw);


    std::vector< std::vector<cv::Point> > cc;

    ConnectComponent::GetCC(bw,cc);

    if(cc.size()<4)
    {
        pxl.assign(cc.size(),cv::Point2f(0,0));

        for(size_t i=0;i<cc.size();i++)
        {
            for(size_t j=0;j<cc[i].size();j++)
            {
                pxl[i].x+=cc[i][j].x;
                pxl[i].y+=cc[i][j].y;
            }
            pxl[i].x/=cc[i].size();
            pxl[i].y/=cc[i].size();
        }
        return false;
    }

    std::vector<size_t> ncc(cc.size(),0);

    for(size_t i=0;i<cc.size();i++)
        ncc[i]=cc[i].size();

    pxl.assign(4,cv::Point2f(0,0));

    for(int i=0;i<4;i++)
    {
        size_t maxi=std::max_element(ncc.begin(),ncc.end())-ncc.begin();

        for(size_t j=0;j<cc[maxi].size();j++)
        {
            pxl[i].x+=cc[maxi][j].x;
            pxl[i].y+=cc[maxi][j].y;
        }
        pxl[i].x/=cc[maxi].size();
        pxl[i].y/=cc[maxi].size();

        cc.erase(cc.begin()+maxi);
        ncc.erase(ncc.begin()+maxi);
    }
    return true;
}

static void LedPos(std::vector<cv::Point3f> &pos, float widthmm=166, float heightmm=107)
{
    pos.assign(4,cv::Point3f(0,0,0));

    pos[1].x=widthmm;
    pos[2].y=heightmm;

    pos[3].x=widthmm;
    pos[3].y=heightmm;
}

static void Measure(const cv::Mat &frame, const cv::Mat &K, const cv::Mat &D, uchar thres=200, float widthmm=166, float heightmm=107)
{

    std::vector<cv::Point2f> pxl;

    cv::Mat mg;
    cv::cvtColor(frame,mg,CV_BGR2GRAY);

    FindLed(pxl,mg,thres);
    if(pxl.size()==4)
    {
        SortLed2(pxl);

        std::vector<cv::Point3f> pos;
        LedPos(pos,widthmm,heightmm);

        cv::Mat R, T;

        cv::solvePnP(pos,pxl,K,D,R,T);


        std::cout<<"R="<<R<<'\n'
                <<"T="<<T<<'\n'
               <<std::flush;

        cv::Mat image;
        frame.copyTo(image);


        cv::circle(image,pxl[0],4,cv::Scalar(255,0,0),3);
        cv::circle(image,pxl[1],4,cv::Scalar(0,255,0),3);
        cv::circle(image,pxl[2],4,cv::Scalar(0,0,255),3);
        cv::circle(image,pxl[3],4,cv::Scalar(0,255,255),3);

        cv::namedWindow("image", cv::WINDOW_NORMAL);
        cv::imshow("image",image);

        return;


    }

    std::cout<<"fail find led\n"
            <<std::flush;

}
static bool FindLed1(const cv::Mat &frame, cv::Point2f &led, double thres=220)
{
    cv::Mat bw;
    if(frame.channels()==1)
    {
        cv::threshold(frame,bw,thres,255,cv::THRESH_BINARY);
    }
    else
    {
        cv::Mat mg;
        cv::cvtColor(frame, mg,CV_BGR2GRAY);
        cv::threshold(mg,bw,thres,255,cv::THRESH_BINARY);
    }



    std::vector< std::vector<cv::Point> > cc;

    ConnectComponent::GetCC(bw,cc);

    if(cc.empty())
        return false;


    std::vector<size_t> ncc(cc.size(),0);

    for(size_t i=0;i<cc.size();i++)
        ncc[i]=cc[i].size();

    size_t maxi=std::max_element(ncc.begin(),ncc.end())-ncc.begin();

    led=cv::Point2f(0,0);

    for(int i=0;i<cc[maxi].size();i++)
    {
        led.x+=cc[maxi][i].x;
        led.y+=cc[maxi][i].y;

    }

    led.x/=cc[maxi].size();
    led.y/=cc[maxi].size();
    return true;
}


static bool FindLed1m(const cv::Mat &frame, cv::Point2f &led, const cv::Mat &mask, double thres=220)
{
    cv::Mat bw;
    if(frame.channels()==1)
    {
        cv::threshold(frame,bw,thres,255,cv::THRESH_BINARY);
    }
    else
    {
        cv::Mat mg;
        cv::cvtColor(frame, mg,CV_BGR2GRAY);
        cv::threshold(mg,bw,thres,255,cv::THRESH_BINARY);
    }


    cv::bitwise_and(bw,mask,bw);

    std::vector< std::vector<cv::Point> > cc;

    ConnectComponent::GetCC(bw,cc);

    if(cc.empty())
        return false;


    std::vector<size_t> ncc(cc.size(),0);

    for(size_t i=0;i<cc.size();i++)
        ncc[i]=cc[i].size();

    size_t maxi=std::max_element(ncc.begin(),ncc.end())-ncc.begin();

    led=cv::Point2f(0,0);

    for(int i=0;i<cc[maxi].size();i++)
    {
        led.x+=cc[maxi][i].x;
        led.y+=cc[maxi][i].y;
    }

    led.x/=cc[maxi].size();
    led.y/=cc[maxi].size();
    return true;
}


static cv::Vec4f Trianglate(cv::Point2f p1, const cv::Mat &K1, const cv::Mat &D1,
                            cv::Point2f p2, const cv::Mat &K2, const cv::Mat &D2,
                            const cv::Mat &rt)
{
    std::vector<cv::Point2f> pl1(1,p1);
    std::vector<cv::Point2f> pl2(1,p2);

    std::vector<cv::Point2f> pl1u(1,p1);
    std::vector<cv::Point2f> pl2u(1,p2);

    cv::undistortPoints(pl1,pl1u,K1,D1);

    cv::undistortPoints(pl2,pl2u,K2,D2);


    cv::Mat P1=(cv::Mat_<double>(3,4)
                <<1,0,0,0,
                0,1,0,0,
                0,0,1,0);

    cv::Mat R;

    cv::Rodrigues(rt.rowRange(0,3),R);

    cv::Mat P2;
    cv::hconcat(R,rt.rowRange(3,6),P2);

    cv::Mat intersect;
    cv::triangulatePoints(P1,P2,pl1u,pl2u,intersect);

    //    std::cout<<"intersect="<<intersect<<'\n'<<intersect.depth()<<'\n'<<intersect.size()<<'\n'<<std::flush;

    return cv::Vec4f(intersect.at<float>(0), intersect.at<float>(1), intersect.at<float>(2), intersect.at<float>(3));
}


static bool Measure2(cv::Vec3f &p,
                     cv::Mat &m1, const cv::Mat &K1, const cv::Mat &D1,
                     cv::Mat &m2, const cv::Mat &K2, const cv::Mat &D2,
                     const cv::Mat &rt, uchar thres1, uchar thres2)
{
    cv::Point2f led1,led2;

    if(FindLed1(m1,led1,thres1) && FindLed1(m2,led2,thres2))
    {

        cv::circle(m1,led1,20,cv::Scalar(255,0,0),3);
        cv::circle(m2,led2,20,cv::Scalar(0,255,0),3);


        cv::Vec4d itc=Trianglate(led1,K1,D1,led2,K2,D2,rt);

        if(itc[3]!=0)
        {
            p[0]=itc[0]/itc[3];
            p[1]=itc[1]/itc[3];
            p[2]=itc[2]/itc[3];

            //            std::cout<<"itc="<<itc<<'\n'<<std::flush;

            return true;

        }


    }



    return false;
}

#endif // MEASURE_HPP
