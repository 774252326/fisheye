#include "cqtopencvviewergl1.h"
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QMouseEvent>


CQtOpenCVViewerGl1::CQtOpenCVViewerGl1(QWidget *parent)
    : QOpenGLWidget(parent)
    , program(0)
{
    memset(textures, 0, sizeof(textures));
}


CQtOpenCVViewerGl1::~CQtOpenCVViewerGl1()
{
    makeCurrent();
    vbo.destroy();
    for (int i = 0; i < 6; ++i)
        delete textures[i];
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
    m.perspective(90, aspectratio, 0.01f,15.f);
//  m.setToIdentity();
    m.translate(0.0f, 0.0f, -1.1f);
//    m.rotate(42*16 / 16.0f, 1.0f, 0.0f, 0.0f);
//    m.rotate(42*16 / 16.0f, 0.0f, 1.0f, 0.0f);
//    m.rotate(42*16 / 16.0f, 0.0f, 0.0f, 1.0f);

    program->setUniformValue("matrix", m);
    program->enableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
    program->enableAttributeArray(PROGRAM_TEXCOORD_ATTRIBUTE);
    program->setAttributeBuffer(PROGRAM_VERTEX_ATTRIBUTE, GL_FLOAT, 0, 3, 5 * sizeof(GLfloat));
    program->setAttributeBuffer(PROGRAM_TEXCOORD_ATTRIBUTE, GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat));


    for (int i = 0; i < 6; ++i) {
        if(textures[i]!=0)
            textures[i]->bind();
        glDrawArrays(GL_TRIANGLE_FAN, i * 4, 4);
    }
}
void CQtOpenCVViewerGl1::resizeGL(int width, int height)
{
//    int side = qMin(width, height);
//    glViewport((width - side) / 2, (height - side) / 2, side, side);
    glViewport(0,0,width,height);
    aspectratio=width;
    aspectratio/=height;
}

void CQtOpenCVViewerGl1::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}

void CQtOpenCVViewerGl1::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();

    if (event->buttons() & Qt::LeftButton) {
        //        rotateBy(8 * dy, 8 * dx, 0);
    } else if (event->buttons() & Qt::RightButton) {
        //        rotateBy(8 * dy, 0, 8 * dx);
    }
    lastPos = event->pos();
}

void CQtOpenCVViewerGl1::mouseReleaseEvent(QMouseEvent * /* event */)
{
    //    emit clicked();
}

void CQtOpenCVViewerGl1::makeObject()
{
    static const int coords[6][4][3] = {
        { { +1, -1, -1 }, { -1, -1, -1 }, { -1, +1, -1 }, { +1, +1, -1 } },
        { { +1, +1, -1 }, { -1, +1, -1 }, { -1, +1, +1 }, { +1, +1, +1 } },
        { { +1, -1, +1 }, { +1, -1, -1 }, { +1, +1, -1 }, { +1, +1, +1 } },
        { { -1, -1, -1 }, { -1, -1, +1 }, { -1, +1, +1 }, { -1, +1, -1 } },
        { { +1, -1, +1 }, { -1, -1, +1 }, { -1, -1, -1 }, { +1, -1, -1 } },
        { { -1, -1, +1 }, { +1, -1, +1 }, { +1, +1, +1 }, { -1, +1, +1 } }
    };

    //    for (int j = 0; j < 6; ++j)
    //        textures[j] = new QOpenGLTexture(QImage().mirrored());

    QVector<GLfloat> vertData;
    for (int i = 0; i < 6; ++i) {
        for (int j = 0; j < 4; ++j) {
            // vertex position
            vertData.append(coords[i][j][0]);
            vertData.append(coords[i][j][1]);
            vertData.append(coords[i][j][2]);
            // texture coordinate
            vertData.append(j == 0 || j == 3);
            vertData.append(j == 0 || j == 1);
        }
    }

    vbo.create();
    vbo.bind();
    vbo.allocate(vertData.constData(), vertData.count() * sizeof(GLfloat));
}

bool CQtOpenCVViewerGl1::showImage(QImage& image)
{

    for (int j = 0; j < 6; ++j)
        if(textures[j]==0)
            textures[j] = new QOpenGLTexture(image);
        else
            textures[j] ->setData(image);

    update();

    return true;
}
