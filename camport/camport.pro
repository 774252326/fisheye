INCLUDEPATH += /usr/local/include \

LIBS += -lopencv_core \
-lopencv_highgui \
-lopencv_imgproc \
-lopencv_imgcodecs \
-lusb \
/home/a/camport_linux/lib_x64/libcamm.so \

HEADERS += \
    depth_render.h \
    percipio_camport.h

SOURCES += \
    depth_render.cpp \
    camport_DeectObs.cpp \
    camport_test.cpp
