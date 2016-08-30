#INCLUDEPATH += C:\opencv2.4.13\opencv\mingw5.3.0\install\include\

#LIBS += C:\opencv2.4.13\opencv\mingw5.3.0\install\x86\mingw\lib\libopencv_calib3d2413.dll.a \
#C:\opencv2.4.13\opencv\mingw5.3.0\install\x86\mingw\lib\libopencv_contrib2413.dll.a \
#C:\opencv2.4.13\opencv\mingw5.3.0\install\x86\mingw\lib\libopencv_core2413.dll.a \
#C:\opencv2.4.13\opencv\mingw5.3.0\install\x86\mingw\lib\libopencv_features2d2413.dll.a \
#C:\opencv2.4.13\opencv\mingw5.3.0\install\x86\mingw\lib\libopencv_flann2413.dll.a \
#C:\opencv2.4.13\opencv\mingw5.3.0\install\x86\mingw\lib\libopencv_gpu2413.dll.a \
#C:\opencv2.4.13\opencv\mingw5.3.0\install\x86\mingw\lib\libopencv_highgui2413.dll.a \
#C:\opencv2.4.13\opencv\mingw5.3.0\install\x86\mingw\lib\libopencv_imgproc2413.dll.a \
#C:\opencv2.4.13\opencv\mingw5.3.0\install\x86\mingw\lib\libopencv_legacy2413.dll.a \
#C:\opencv2.4.13\opencv\mingw5.3.0\install\x86\mingw\lib\libopencv_ml2413.dll.a \
#C:\opencv2.4.13\opencv\mingw5.3.0\install\x86\mingw\lib\libopencv_nonfree2413.dll.a \
#C:\opencv2.4.13\opencv\mingw5.3.0\install\x86\mingw\lib\libopencv_objdetect2413.dll.a \
#C:\opencv2.4.13\opencv\mingw5.3.0\install\x86\mingw\lib\libopencv_ocl2413.dll.a \
#C:\opencv2.4.13\opencv\mingw5.3.0\install\x86\mingw\lib\libopencv_photo2413.dll.a \
#C:\opencv2.4.13\opencv\mingw5.3.0\install\x86\mingw\lib\libopencv_stitching2413.dll.a \
#C:\opencv2.4.13\opencv\mingw5.3.0\install\x86\mingw\lib\libopencv_superres2413.dll.a \
#C:\opencv2.4.13\opencv\mingw5.3.0\install\x86\mingw\lib\libopencv_ts2413.a \
#C:\opencv2.4.13\opencv\mingw5.3.0\install\x86\mingw\lib\libopencv_video2413.dll.a \
#C:\opencv2.4.13\opencv\mingw5.3.0\install\x86\mingw\lib\libopencv_videostab2413.dll.a \



INCLUDEPATH += /usr/local/include \

LIBS += /usr/local/lib/libopencv_calib3d.so.3.1.0 \
/usr/local/lib/libopencv_photo.so.3.1.0 \
/usr/local/lib/libopencv_core.so.3.1.0 \
/usr/local/lib/libopencv_shape.so.3.1.0 \
/usr/local/lib/libopencv_features2d.so.3.1.0 \
/usr/local/lib/libopencv_stitching.so.3.1.0 \
/usr/local/lib/libopencv_flann.so.3.1.0 \
/usr/local/lib/libopencv_superres.so.3.1.0 \
/usr/local/lib/libopencv_highgui.so.3.1.0 \
/usr/local/lib/libopencv_videoio.so.3.1.0 \
/usr/local/lib/libopencv_imgcodecs.so.3.1.0 \
/usr/local/lib/libopencv_video.so.3.1.0 \
/usr/local/lib/libopencv_imgproc.so.3.1.0 \
/usr/local/lib/libopencv_videostab.so.3.1.0 \
/usr/local/lib/libopencv_ml.so.3.1.0 \
/usr/local/lib/libopencv_objdetect.so.3.1.0 \


SOURCES += \
    main.cpp \
    ocam_functions.cpp

HEADERS += \
    ocam_functions.h \
    stereoocam.hpp
