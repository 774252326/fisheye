#ifndef CQTOPENCVVIEWERGL_H
#define CQTOPENCVVIEWERGL_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_2_0>
#include <QOpenGLBuffer>

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram);
QT_FORWARD_DECLARE_CLASS(QOpenGLTexture)

class CQtOpenCVViewerGl1 : public QOpenGLWidget, protected QOpenGLFunctions_2_0
{
    Q_OBJECT
public:
    explicit CQtOpenCVViewerGl1(QWidget *parent = 0);

    ~CQtOpenCVViewerGl1();
    bool showImage(QImage& image); /// Used to set the image to be viewed


protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    void makeObject();

    static void projection(float &pixelx, float &pixely, float alpha, float beta)
    {
         float centerx=640/2;
         float centery=480/2;
         float imgradius=centery;
         float halffov=110*acos(-1)/180;
         pixelx=centerx-imgradius*alpha/halffov*cos(beta);
         pixely=centery+imgradius*alpha/halffov*sin(beta);
    }

    static void fisheyemap(float** &vertex, int** &tgstripindex,
                           float halffov, int nsega, int nsegb,
                           float width, float height, float ballradius=1)
    {
        vertex=new float*[nsega*nsegb+1];
        for(int i=0;i<nsega*nsegb+1;i++)
            vertex[i]=new float[5];
        tgstripindex=new int*[nsegb];
        for(int i=0;i<nsegb;i++)
            tgstripindex[i]=new int[nsega*2+1];

        int ai=0;
        int bi=0;
        float alpha=ai*halffov/nsega;
        float beta=bi*2*acos(-1)/nsegb;

        vertex[0][0]=ballradius*sin(alpha)*cos(beta);
        vertex[0][1]=ballradius*sin(alpha)*sin(beta);
        vertex[0][2]=ballradius*cos(alpha);
        projection(vertex[0][3], vertex[0][4], alpha, beta);
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
                projection(vertex[vertexindex][3], vertex[vertexindex][4], alpha, beta);
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

    static void fisheyevertex(std::vector<float> &tgstripvertex, std::vector<int> &tgstripseg,
            float halffov, int nsega, int nsegb,
                              float width, float height, float ballradius=1)
    {
        float** vertex;
        int** tgstripindex;

        fisheyemap(vertex, tgstripindex, halffov, nsega, nsegb, width, height, ballradius);

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

    QPoint lastPos;
    QOpenGLTexture *texture;
    QOpenGLShaderProgram *program;
    QOpenGLBuffer vbo;

    float winw;
    float winh;

    float fovh;

    float angle;
    float anglex;


    std::vector<int> tgstripseg;

};

#endif // CQTOPENCVVIEWERGL_H
