
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
