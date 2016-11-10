#ifndef DEPTHTHREAD_HPP
#define DEPTHTHREAD_HPP
#include <QtCore>
#include "percipio_camport.h"
#include <opencv2/opencv.hpp>
#include <stdlib.h>
#include "depth_render.h"

#include "controlthread.hpp"

//#define DPDEPTH

class DepthThread : public QThread
{
public:
    //    bool *pobjflag;
    bool goflag;

    ControlThread *pct;

    DepthThread(ControlThread *pct0)
        : goflag(false)
        , pct(pct0)
    {

    }


    void CopyBuffer(percipio::ImageBuffer *pbuf, cv::Mat &img) {
        switch (pbuf->type) {
        case percipio::ImageBuffer::PIX_8C1:
            img.create(pbuf->height, pbuf->width, CV_8UC1);
            break;
        case percipio::ImageBuffer::PIX_16C1:
            img.create(pbuf->height, pbuf->width, CV_16UC1);
            break;
        case percipio::ImageBuffer::PIX_8C3:
            img.create(pbuf->height, pbuf->width, CV_8UC3);
            break;
        case percipio::ImageBuffer::PIX_32FC3:
            img.create(pbuf->height, pbuf->width, CV_32FC3);
            break;
        default:
            img.release();
            return;
        }
        memcpy(img.data, pbuf->data, pbuf->width * pbuf->height * pbuf->get_pixel_size());
    }


    bool CheckInRange(const cv::Mat &m, float minValue, float maxValue)
    {
        for(size_t i=0;i<m.total();i++)
        {
            unsigned short d = m.at<ushort>(i);
            if(d>minValue && d<maxValue)
                return true;
        }
        return false;
    }


    void run()
    {
        goflag=true;
#ifdef DPDEPTH
        DepthRender render;
        render.range_mode = DepthRender::COLOR_RANGE_DYNAMIC;
        render.color_type = DepthRender::COLORTYPE_BLUERED;
        render.invalid_label = 0;
        render.Init();
#endif
        percipio::DepthCameraDevice port(percipio::MODEL_DPB04GN);
        percipio::SetLogLevel(percipio::LOG_LEVEL_INFO);
        port.SetCallbackUserData(NULL);
        port.SetFrameReadyCallback(NULL);

        int ver = percipio::LibVersion();
        printf("Sdk version is %d\n", ver);

        int ret = port.OpenDevice();
        if (percipio::CAMSTATUS_SUCCESS != ret) {
            printf("open device failed\n");
            return ;
        }

        int wait_time;
        port.GetProperty(percipio::PROP_WAIT_NEXTFRAME_TIMEOUT, (char *)&wait_time, sizeof(int));
        printf("get property PROP_WAIT_NEXTFRAME_TIMEOUT %d\n", wait_time);

        int reti = port.SetProperty_Int(percipio::PROP_WORKMODE, percipio::WORKMODE_DEPTH);
        //int reti = port.SetProperty_Int(percipio::PROP_WORKMODE, percipio::WORKMODE_IR);
        //int reti = port.SetProperty_Int(percipio::PROP_WORKMODE, percipio::WORKMODE_IR_DEPTH);
        if (reti < 0) {
            printf("set mode failed,error code:%d\n", reti);
            return ;
        }



        while(goflag){
            if (port.FramePackageGet() == percipio::CAMSTATUS_SUCCESS) {
                percipio::ImageBuffer pimage;


                if (percipio::CAMSTATUS_SUCCESS == port.FrameGet(percipio::CAMDATA_DEPTH, &pimage)) {
                    cv::Mat depth;
                    CopyBuffer(&pimage, depth);
                    pct->SetObj(CheckInRange(depth,600,900));

                    std::cout << "depth thread\n"<<std::flush;
#ifdef DPDEPTH
                    cv::Mat t;
                    render.Compute(depth, t);
                    cv::imshow("depth", t);
                    msleep(1);
#endif
                }
            }


        }


        port.CloseDevice();
#ifdef DPDEPTH
        render.Uninit();
#endif
    }
};

#endif // DEPTHTHREAD_HPP
