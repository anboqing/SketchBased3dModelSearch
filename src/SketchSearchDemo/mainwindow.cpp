
#include "mainwindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QList>
#include <QMenu>
#include <QMenuBar>
#include <QFileDialog>

#include "sketchsearcher.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    searchEngine = SketchSearcher::getInstance();

    setupMenuBar();

    sketchArea = new SketchArea;
    sketchArea->setAutoSaveSketch(true);
    triMeshView = new TriMeshView;

    QSplitter *leftSplitter = new QSplitter(Qt::Vertical);
    leftSplitter->addWidget(sketchArea);
    leftSplitter->addWidget(triMeshView);

    resultPhotoWidget = new ResultPhotoWidget;
    QSplitter *mainSplitter = new QSplitter(Qt::Horizontal);
    mainSplitter->addWidget(leftSplitter);
    mainSplitter->addWidget(resultPhotoWidget);

    QSize size = this->size();
    QList<int> leftList;
    leftList.push_back(size.height()/2);
    leftList.push_back(size.height()/2);
    leftSplitter->setSizes(leftList);

    QList<int> mainList;
    mainList.push_back(size.width()/7);
    mainList.push_back(size.width()/3);
    mainSplitter->setSizes(mainList);

    this->setCentralWidget(mainSplitter);

    this->setMinimumSize(800, 600);

    //auto search
    connect(sketchArea, SIGNAL(newSketchDone(QString)), this, SLOT(search(QString)));

    connect(sketchArea, SIGNAL(clearSketchDone()), this, SLOT(clearResults()));
    connect(resultPhotoWidget, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(showLineDrawing(QTableWidgetItem*)));

}

void MainWindow::setupMenuBar()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

    QAction *openAction = fileMenu->addAction(tr("&Open"));
    connect(openAction, SIGNAL(triggered()), this, SLOT(openFile()));

    QAction *queryAction = menuBar()->addAction(tr("&Query"));
    connect(queryAction, SIGNAL(triggered()), this, SLOT(query()));
}

void MainWindow::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::currentPath(),
                                                    tr("Images (*.png *.bmp *.jpg)"));

    if(!fileName.isEmpty())
    {
        sketchArea->openImage(fileName);
    }
}

void MainWindow::query()
{
    QueryResults results;
    searchEngine->query(sketchArea->sketchPath().toStdString(), results);
    resultPhotoWidget->updateResults(results);
}

void MainWindow::search(const QString &fileName)
{
    QueryResults results;
    searchEngine->query(fileName.toStdString(), results);
    resultPhotoWidget->updateResults(results);
}

void MainWindow::clearResults()
{
    QueryResults res;
    resultPhotoWidget->updateResults(res);
    triMeshView->clearMesh();
}

void MainWindow::showLineDrawing(QTableWidgetItem *item)
{
    QString fileName = item->whatsThis();

    int sketch_idx = item->text().toInt() ;

    std::string& model_name_str = this->searchEngine->get_model_filename_list()[searchEngine->get_sketch_model_mapping()[sketch_idx]];
    cout << model_name_str << endl;

    QString modelFileName = QString(model_name_str.c_str());

    triMeshView->readMesh(modelFileName.toStdString().c_str(),
                                "");

}

MainWindow::~MainWindow()
{

}
