#include "cqtopencvviewergl1.h"
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QMouseEvent>

#include <iostream>

CQtOpenCVViewerGl1::CQtOpenCVViewerGl1(QWidget *parent)
    : QOpenGLWidget(parent)
    , program(0)
{
    texture=0;
    angle=0;
    anglex=-120;
    fovh=80;
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
        m.rotate(anglex, 1.0f, 0.0f, 0.0f);
//        m.rotate(angle, 0.0f, 1.0f, 0.0f);
    m.rotate(angle, 0.0f, 0.0f, 1.0f);

    program->setUniformValue("matrix", m);
    program->enableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
    program->enableAttributeArray(PROGRAM_TEXCOORD_ATTRIBUTE);
    program->setAttributeBuffer(PROGRAM_VERTEX_ATTRIBUTE, GL_FLOAT, 0, 3, 5 * sizeof(GLfloat));
    program->setAttributeBuffer(PROGRAM_TEXCOORD_ATTRIBUTE, GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat));



    if(texture!=0)
        texture->bind();
//    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    int starti=0;
    for(int i=0;i<tgstripseg.size();i++)
    {
        glDrawArrays(GL_TRIANGLE_STRIP, starti, tgstripseg[i]);
        starti+=tgstripseg[i];
    }

}
void CQtOpenCVViewerGl1::resizeGL(int width, int height)
{
    //    int side = qMin(width, height);
    //    glViewport((width - side) / 2, (height - side) / 2, side, side);
    glViewport(0,0,width,height);
    winw=width;
    winh=height;
}

void CQtOpenCVViewerGl1::mousePressEvent(QMouseEvent *event)
{
    std::cout<<"\n\nmousePressEvent\n\n"<<std::flush;
    lastPos = event->pos();
}

void CQtOpenCVViewerGl1::mouseMoveEvent(QMouseEvent *event)
{
    std::cout<<"\n\nmouseMoveEvent\n\n"<<std::flush;
    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();

    if (event->buttons() & Qt::LeftButton) {
        //        rotateBy(8 * dy, 8 * dx, 0);
        angle-=dx*fovh/winh;
        anglex-=dy*fovh/winh;
    } else if (event->buttons() & Qt::RightButton) {
        //        rotateBy(8 * dy, 0, 8 * dx);
    }
    lastPos = event->pos();

    std::cout<<"anglex="<<anglex<<"\n"<<std::flush;
    std::cout<<"angle="<<angle<<"\n"<<std::flush;

}

void CQtOpenCVViewerGl1::mouseReleaseEvent(QMouseEvent *  event )
{
    std::cout<<"\n\nmouseReleaseEvent\n\n"<<std::flush;
    //    emit clicked();

//    if (event->buttons() & Qt::LeftButton) {
//        angle+=10;
//    } else if (event->buttons() & Qt::RightButton) {
//        angle-=10;
//    }

//    std::cout<<"angle="<<angle<<"\n"<<std::flush;
}

void CQtOpenCVViewerGl1::makeObject()
{
    static const float coords[4][5] = {
         { +1, -1, -1, 1, 0 },
        { +1, +1, -1, 1, 1 },
        { -1, +1, -1, 0, 1 },
        { -1, -1, -1, 0, 0 }
    };

//    QVector<GLfloat> vertData;

    std::vector<float> vertData;
    for (int j = 0; j < 4; ++j) {
//        // vertex position
//        vertData.append(coords[j][0]);
//        vertData.append(coords[j][1]);
//        vertData.append(coords[j][2]);
//        // texture coordinate
//        vertData.append(coords[j][3]);
//        vertData.append(coords[j][4]);

        // vertex position
        vertData.push_back(coords[j][0]);
        vertData.push_back(coords[j][1]);
        vertData.push_back(coords[j][2]);
        // texture coordinate
        vertData.push_back(coords[j][3]);
        vertData.push_back(coords[j][4]);
    }

    std::vector<float> v;
    fisheyevertex(v, tgstripseg, 110*acos(-1)/180, 40, 90, 640, 480);

    vbo.create();
    vbo.bind();
//    vbo.allocate(vertData.constData(), vertData.count() * sizeof(GLfloat));
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
