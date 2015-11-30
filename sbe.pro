TEMPLATE = app
CONFIG += console c++11

CONFIG -= app_bundle
CONFIG -= qt
TARGET = feature_test

SOURCES += \
    src/detector.cpp \
    src/galif.cpp \

HEADERS += \
    include/config.hpp \
    include/typedefs.h \
    include/feature.h \
    include/detector.h \
    include/galif.h \


INCLUDEPATH += $$PWD \
    /usr/local/include \
    /usr/local/include/boost \
    /home/anboqing/soft/opencv-2.4.9/include \
    /home/anboqing/soft/opencv-2.4.9/include/opencv \
    /home/anboqing/soft/opencv-2.4.9/include/opencv2 \

LIBS += -L/usr/local/lib/ \
     `pkg-config --libs opencv` \
    -lboost_thread -lboost_system -lboost_filesystem
