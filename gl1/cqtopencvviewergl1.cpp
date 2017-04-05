#include "cqtopencvviewergl1.h"
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QMouseEvent>

#include <iostream>

#include "fisheyesphere.hpp"

CQtOpenCVViewerGl1::CQtOpenCVViewerGl1(QWidget *parent)
    : QOpenGLWidget(parent)
    , program(0)
{
    texture=0;
    anglex=0;
    angley=0;

    fovh=120;
}


CQtOpenCVViewerGl1::~CQtOpenCVViewerGl1()
{
    makeCurrent();
    vbo.destroy();
    if(texture!=0)
        delete texture;
    delete program;
    doneCurrent();
}


void CQtOpenCVViewerGl1::initializeGL()
{
    initializeOpenGLFunctions();

    makeObject();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_TEXCOORD_ATTRIBUTE 1

    QOpenGLShader *vshader = new QOpenGLShader(QOpenGLShader::Vertex, this);
    const char *vsrc =
            "attribute highp vec4 vertex;\n"
            "attribute mediump vec4 texCoord;\n"
            "varying mediump vec4 texc;\n"
            "uniform mediump mat4 matrix;\n"
            "void main(void)\n"
            "{\n"
            "    gl_Position = matrix * vertex;\n"
            "    texc = texCoord;\n"
            "}\n";
    vshader->compileSourceCode(vsrc);

    QOpenGLShader *fshader = new QOpenGLShader(QOpenGLShader::Fragment, this);
    const char *fsrc =
            "uniform sampler2D texture;\n"
            "varying mediump vec4 texc;\n"
            "void main(void)\n"
            "{\n"
            "    gl_FragColor = texture2D(texture, texc.st);\n"
            "}\n";
    fshader->compileSourceCode(fsrc);

    program = new QOpenGLShaderProgram;
    program->addShader(vshader);
    program->addShader(fshader);
    program->bindAttributeLocation("vertex", PROGRAM_VERTEX_ATTRIBUTE);
    program->bindAttributeLocation("texCoord", PROGRAM_TEXCOORD_ATTRIBUTE);
    program->link();

    program->bind();
    program->setUniformValue("texture", 0);
}

void CQtOpenCVViewerGl1::paintGL()
{
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    QMatrix4x4 m;

    //    m.ortho(-0.5f, +0.5f, +0.5f, -0.5f, 0.01f, 15.0f);
    m.perspective(fovh, winw/winh, 0.01f,15.f);
    //  m.setToIdentity();
    //    m.translate(0.0f, 0.0f, -1.1f);
    m.rotate(angley, 1.0f, 0.0f, 0.0f);
    //        m.rotate(angle, 0.0f, 1.0f, 0.0f);
    m.rotate(anglex, 0.0f, 0.0f, 1.0f);

    program->setUniformValue("matrix", m);
    program->enableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
    program->enableAttributeArray(PROGRAM_TEXCOORD_ATTRIBUTE);
    program->setAttributeBuffer(PROGRAM_VERTEX_ATTRIBUTE, GL_FLOAT, 0, 3, 5 * sizeof(GLfloat));
    program->setAttributeBuffer(PROGRAM_TEXCOORD_ATTRIBUTE, GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat));



    if(texture!=0)
        texture->bind();
    int starti=0;
    for(int i=0;i<tgstripseg.size();i++)
    {
        glDrawArrays(GL_TRIANGLE_STRIP, starti, tgstripseg[i]);
        starti+=tgstripseg[i];
    }

}
void CQtOpenCVViewerGl1::resizeGL(int width, int height)
{
    glViewport(0,0,width,height);
    winw=width;
    winh=height;
}

void CQtOpenCVViewerGl1::mousePressEvent(QMouseEvent *event)
{
//    std::cout<<"\n\nmousePressEvent\n\n"<<std::flush;
    lastPos = event->pos();
}

void CQtOpenCVViewerGl1::mouseMoveEvent(QMouseEvent *event)
{
//    std::cout<<"\n\nmouseMoveEvent\n\n"<<std::flush;
    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();

    if (event->buttons() & Qt::LeftButton) {
        anglex+=dx*fovh/winh;
        angley-=dy*fovh/winh;
        if(angley<0)angley=0;
        if(angley>180)angley=180;

        update();
    } else
        if (event->buttons() & Qt::RightButton) {
        }
    lastPos = event->pos();

    std::cout<<"anglex="<<anglex<<"\n"<<std::flush;
    std::cout<<"angley="<<angley<<"\n"<<std::flush;



}

void CQtOpenCVViewerGl1::mouseReleaseEvent(QMouseEvent *  event )
{
//    std::cout<<"\n\nmouseReleaseEvent\n\n"<<std::flush;
    update();
}

void CQtOpenCVViewerGl1::makeObject()
{

    std::vector<float> v;
    FisheyeSphere(1280,1024,110*acos(-1)/180).fisheyevertex(v, tgstripseg);

    vbo.create();
    vbo.bind();
    vbo.allocate(v.data(), v.size() * sizeof(GLfloat));
}

bool CQtOpenCVViewerGl1::showImage(QImage& image)
{


    if(texture==0)
        texture = new QOpenGLTexture(image);
    else
    {
        texture->release();
        delete texture;
        texture = new QOpenGLTexture(image);
        //            texture ->setData(image, QOpenGLTexture::DontGenerateMipMaps);
    }

    update();

    return true;
}
