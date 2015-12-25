#-------------------------------------------------------------------------
# Copyright (c) 2014 Zhang Dongdong
# All rights reserved.
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
# http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#-------------------------------------------------------------------------
#-------------------------------------------------
#
# Project created by QtCreator 2014-03-12T08:55:19
#
#-------------------------------------------------

QT       += core gui \
            opengl \
            printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SketchSearchDemo
TEMPLATE = app

DESTDIR = ../bin

include(trimeshview/trimeshview.pri)

SOURCES += main.cpp\
        mainwindow.cpp \
    sketcharea.cpp \
    resultphotowidget.cpp \
    sketchsearcher.cpp \
    config.cpp \
    detector.cpp \
    galif.cpp


HEADERS  += mainwindow.h \
    sketcharea.h \
    searchengine.h \
    resultphotowidget.h \
    sketchsearcher.h \
    file_tool.h \
    config.h \
    typedefs.h \
    Timer.h \
    quantize.h \
    galif.h \
    detector.h \
    feature.h
QMAKE_CXXFLAGS += -std=c++11

INCLUDEPATH += $$PWD \
    /usr/local/include \
    /usr/local/include/boost \
    /home/anboqing/soft/opencv-2.4.9/include \
    /home/anboqing/soft/opencv-2.4.9/include/opencv \
    /home/anboqing/soft/opencv-2.4.9/include/opencv2 \

LIBS += -L/usr/local/lib/ \
    -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_features2d -lopencv_ml \
    -lboost_thread -lboost_system -lboost_filesystem -lglog
