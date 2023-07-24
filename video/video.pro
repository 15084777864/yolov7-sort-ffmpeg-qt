QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++14

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    widget.cpp \
    videoplayer.cpp

HEADERS += \
    widget.h \
    videoplayer.h

FORMS += \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

include(./tracker/tracker.pri)
include(./yolo/yolo.pri)

INCLUDEPATH += /usr/local/ffmpeg/include

LIBS += -L"/usr/local/ffmpeg/lib/"
LIBS += -lavutil \
        -lavcodec \
        -lavdevice \
        -lavfilter \
        -lavformat \
        -lavutil \
        -lswresample \
        -lswscale


INCLUDEPATH += /usr/local/include/opencv4/opencv2 \
               /usr/local/include/opencv4\


LIBS += /usr/local/lib/libopencv_*


#INCLUDEPATH += /usr/src/tensorrt/samples/common/
INCLUDEPATH += /usr/local/cuda/include/
INCLUDEPATH += /usr/lib/x86_64-linux-gnu/
INCLUDEPATH += /home/tuya/TensorRT-8.6.1.6/targets/x86_64-linux-gnu/include/

LIBS += -L/home/tuya/TensorRT-8.6.1.6/targets/x86_64-linux-gnu/lib/
LIBS += -L/usr/local/cuda/lib64/
LIBS += -L/usr/local/cuda-11.8/targets/x86_64-linux/lib/
LIBS += -L/usr/local/cuda-11.8/targets/x86_64-linux/lib/stubs/

LIBS += -lnvinfer
LIBS +=-lnvparsers
LIBS +=-lnvinfer_plugin
LIBS +=-lnvonnxparser
LIBS +=-lrt
LIBS +=-ldl
LIBS +=-lpthread
LIBS +=-lcudnn
LIBS +=-lcublas
LIBS +=-lcudart
LIBS +=-lculibos



