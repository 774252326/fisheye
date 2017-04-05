#-------------------------------------------------
#
# Project created by QtCreator 2017-03-27T15:30:59
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = gl1
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    cqtopencvviewergl1.cpp

HEADERS  += mainwindow.h \
    cqtopencvviewergl1.h \
    fisheyesphere.hpp

FORMS    += mainwindow.ui

INCLUDEPATH += C:/opencv/mingw/include \

LIBS += \
C:/opencv/mingw/x86/mingw/lib/libopencv_calib3d320.dll.a \
C:/opencv/mingw/x86/mingw/lib/libopencv_core320.dll.a \
C:/opencv/mingw/x86/mingw/lib/libopencv_features2d320.dll.a \
C:/opencv/mingw/x86/mingw/lib/libopencv_flann320.dll.a \
C:/opencv/mingw/x86/mingw/lib/libopencv_highgui320.dll.a \
C:/opencv/mingw/x86/mingw/lib/libopencv_imgcodecs320.dll.a \
C:/opencv/mingw/x86/mingw/lib/libopencv_imgproc320.dll.a \
C:/opencv/mingw/x86/mingw/lib/libopencv_ml320.dll.a \
C:/opencv/mingw/x86/mingw/lib/libopencv_objdetect320.dll.a \
C:/opencv/mingw/x86/mingw/lib/libopencv_photo320.dll.a \
C:/opencv/mingw/x86/mingw/lib/libopencv_shape320.dll.a \
C:/opencv/mingw/x86/mingw/lib/libopencv_stitching320.dll.a \
C:/opencv/mingw/x86/mingw/lib/libopencv_superres320.dll.a \
C:/opencv/mingw/x86/mingw/lib/libopencv_video320.dll.a \
C:/opencv/mingw/x86/mingw/lib/libopencv_videoio320.dll.a \
C:/opencv/mingw/x86/mingw/lib/libopencv_videostab320.dll.a
