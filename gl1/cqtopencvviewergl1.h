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
    void wheelEvent(QWheelEvent *event) override;

private:
    void makeObject();

    QPoint lastPos;
    QOpenGLTexture *texture;
    QOpenGLShaderProgram *program;
    QOpenGLBuffer vbo;

    float winw;
    float winh;

    float fovh;

    float angley;
    float anglex;


    std::vector<int> tgstripseg;

};

#endif // CQTOPENCVVIEWERGL_H
