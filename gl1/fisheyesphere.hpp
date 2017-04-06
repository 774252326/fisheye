#ifndef FISHEYESPHERE_HPP
#define FISHEYESPHERE_HPP

#include <opencv2/opencv.hpp>

class FisheyeSphere
{
protected:
    float halffov;
    int nsega;
    int nsegb;
    float width;
    float height;
    float f;
public:
    FisheyeSphere(float width0, float height0, float halffov0, int nsega0=40, int nsegb0=90)
        : halffov(halffov0)
        , width(width0)
        , height(height0)
        , nsega(nsega0)
        , nsegb(nsegb0)
    {
//                float imageRadiusPixel=ImageRadiusPixel(0.75, 4.28, 4.3/2);
        float imageRadiusPixel=ImageRadiusPixel(0.75, 4.55, 4.2/2);

//                float f_equidistant=imageRadiusPixel/halffov;f=f_equidistant;
        //        float f_orthographic=imageRadiusPixel/sin(halffov);f=f_orthographic;
        float f_equisolid=imageRadiusPixel/sin(halffov/2);f=f_equisolid;
//                float f_stereographic=imageRadiusPixel/tan(halffov/2);f=f_stereographic;
    }

    float ImageRadiusPixel(float cmosH2W, float cmosHeightmm, float imageRadiusmm)
    {
        float pixelPermm;
        if(height/width<cmosH2W)
            pixelPermm=width*cmosH2W/cmosHeightmm;
        else
            pixelPermm=height/cmosHeightmm;
        return imageRadiusmm*pixelPermm;
    }

    virtual float AlphaToRadius(float alpha)
    {
//                return alpha*f;
        return sin(alpha/2)*f;
//        return sin(alpha)*f;
//        return tan(alpha/2)*f;

    }

    void projection(float &pixelx, float &pixely, float alpha, float beta, float centerx, float centery)
    {
        float r=AlphaToRadius(alpha);
        pixelx=centerx-r*cos(beta);
        pixely=centery+r*sin(beta);
    }

    void FixOrient(float &x, float &y, float &z, float gravityx, float gravityy, float gravityz)
    {
        cv::Vec3f gravity(gravityx, gravityy, gravityz);
        cv::Vec3f axis=gravity.cross(cv::Vec3f(0,0,1));
        cv::Vec3f p(x,y,z);
        double absgravity=cv::norm(gravity);
        double absaxis=cv::norm(axis);
        if(absaxis==0)
        {
            if(gravityz<0)
            {
                y=-y;
                z=-z;
            }
            return;
        }
        double rad=acos(gravityz/absgravity);
        cv::Vec3f rv=axis*rad/absaxis;
        cv::Matx33f rm;
        cv::Rodrigues(rv,rm);
        cv::Vec3f pout=rm*p;
        x=pout[0];
        y=pout[1];
        z=pout[2];
    }

    void fisheyemap(float** &vertex, int** &tgstripindex, float ballradius=1)
    {
        vertex=new float*[nsega*nsegb+1];
        for(int i=0;i<nsega*nsegb+1;i++)
            vertex[i]=new float[5];
        tgstripindex=new int*[nsegb];
        for(int i=0;i<nsegb;i++)
            tgstripindex[i]=new int[nsega*2+1];

        float centerx=width/2;
        float centery=height/2;

        int ai=0;
        int bi=0;
        float alpha=ai*halffov/nsega;
        float beta=bi*2*acos(-1)/nsegb;

        vertex[0][0]=ballradius*sin(alpha)*cos(beta);
        vertex[0][1]=ballradius*sin(alpha)*sin(beta);
        vertex[0][2]=ballradius*cos(alpha);

        FixOrient(vertex[0][0], vertex[0][1], vertex[0][2], 0, -1, -0);

        projection(vertex[0][3], vertex[0][4], alpha, beta, centerx, centery);
        vertex[0][3]/=width;
        vertex[0][4]/=height;

        for(ai=1;ai<=nsega;ai++)
        {
            for(bi=0;bi<nsegb;bi++)
            {
                alpha=ai*halffov/nsega;
                beta=bi*2*acos(-1)/nsegb;
                int vertexindex=(ai-1)*nsegb+bi+1;
                vertex[vertexindex][0]=ballradius*sin(alpha)*cos(beta);
                vertex[vertexindex][1]=ballradius*sin(alpha)*sin(beta);
                vertex[vertexindex][2]=ballradius*cos(alpha);

                FixOrient(vertex[vertexindex][0], vertex[vertexindex][1], vertex[vertexindex][2], 0, -1, -0);

                projection(vertex[vertexindex][3], vertex[vertexindex][4], alpha, beta, centerx, centery);
                vertex[vertexindex][3]/=width;
                vertex[vertexindex][4]/=height;
            }
        }


        for(bi=0;bi<nsegb-1;bi++)
        {
            tgstripindex[bi][0]=0;
            for(ai=1;ai<=nsega;ai++)
            {
                tgstripindex[bi][2*ai]=(ai-1)*nsegb+bi+1;
                tgstripindex[bi][2*ai-1]=(ai-1)*nsegb+bi+2;
            }
        }

        bi=nsegb-1;
        tgstripindex[bi][0]=0;
        for(ai=1;ai<=nsega;ai++)
        {
            tgstripindex[bi][2*ai]=(ai-1)*nsegb+bi+1;
            tgstripindex[bi][2*ai-1]=(ai-1)*nsegb+1;
        }

    }


    void fisheyevertex(std::vector<float> &tgstripvertex, std::vector<int> &tgstripseg, float ballradius=1)
    {
        float** vertex;
        int** tgstripindex;

        fisheyemap(vertex, tgstripindex, ballradius);

        tgstripseg.assign(nsegb, nsega*2+1);
        tgstripvertex.assign(5*nsegb*(nsega*2+1),0);

        for(int bi=0;bi<nsegb;bi++)
        {
            for(int j=0;j<nsega*2+1;j++)
            {
                int pindex=bi*(nsega*2+1)+j;
                int ppindex=tgstripindex[bi][j];
                for(int i=0;i<5;i++)
                    tgstripvertex[5*pindex+i]=vertex[ppindex][i];
            }
        }

        for(int i=0;i<nsega*nsegb+1;i++)
            delete [](vertex[i]);
        delete []vertex;
        for(int i=0;i<nsegb;i++)
            delete [](tgstripindex[i]);
        delete []tgstripindex;
    }


};

#endif // FISHEYESPHERE_HPP
