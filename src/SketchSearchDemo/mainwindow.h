/*************************************************************************
 * Copyright (c) 2014 Zhang Dongdong
 * All rights reserved.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
**************************************************************************/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "sketcharea.h"
#include "searchengine.h"
#include "resultphotowidget.h"
#include "trimeshview.h"
#include "sketchsearcher.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
private slots:
    void openFile();
    void query();
    void search(const QString &fileName);
    void showLineDrawing(QTableWidgetItem *item);
    void clearResults();
private:
    void setupMenuBar();
private:
    SketchArea *sketchArea;
    TriMeshView *triMeshView;
    ResultPhotoWidget *resultPhotoWidget;
    SketchSearcher *searchEngine;
};

#endif // MAINWINDOW_H