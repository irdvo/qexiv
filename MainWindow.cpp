#include <QApplication>
#include <QTextEdit>
#include <QDockWidget>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QLabel>
#include <QScrollArea>
#include <QFileDialog>
#include <QImageReader>
#include <QScrollBar>
#include <QTableView>
#include <QHeaderView>

#if QT_VERSION >= 0x050000
#include <QStandardPaths>
#endif

#include "MainWindow.h"


MainWindow::MainWindow() :
  _imageDialog(0),
  _imageFilename(""),
  _exiv2(this)
{
  _imageLabel = new QLabel;
    _imageLabel->setBackgroundRole(QPalette::Base);
    _imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    _imageLabel->setScaledContents(true);
    _imageLabel->setAlignment(Qt::AlignCenter);

  _imageScrollArea = new QScrollArea;
    _imageScrollArea->setBackgroundRole(QPalette::Base);
    _imageScrollArea->setWidget(_imageLabel);
    _imageScrollArea->setVisible(true);
    _imageScrollArea->setAlignment(Qt::AlignCenter);

  _scaleFactor = 1.0;

  setCentralWidget(_imageScrollArea);

  createActions();
  createMenus();
  createToolBars();
  createStatusBar();
  createDockWindows();

  setTitle();
}

MainWindow::~MainWindow()
{

}

// -- GUI ---------------------------------------------------------------------
void MainWindow::createActions()
{
  _openFileAction = new QAction(tr("&Open File..."), this);
  _openFileAction->setStatusTip(tr("Open an image file"));
  connect(_openFileAction, SIGNAL(triggered()), this, SLOT(openFile()));

  _quitAction = new QAction(tr("&Quit"), this);
  _quitAction->setShortcuts(QKeySequence::Quit);
  _quitAction->setStatusTip(tr("Quit the application"));
  connect(_quitAction, SIGNAL(triggered()), this, SLOT(close()));

  _aboutAction = new QAction(tr("&About"), this);
  _aboutAction->setStatusTip(tr("Show the application's About box"));
  connect(_aboutAction, SIGNAL(triggered()), this, SLOT(about()));

  _aboutQtAction = new QAction(tr("About &Qt"), this);
  _aboutQtAction->setStatusTip(tr("Show the Qt library's About box"));
  connect(_aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void MainWindow::createMenus()
{
  _fileMenu = menuBar()->addMenu(tr("&File"));
  _fileMenu->addAction(_openFileAction);
  _fileMenu->addSeparator();
  _fileMenu->addAction(_quitAction);

  _imageMenu = menuBar()->addMenu(tr("&Image"));

  _zoomInAction = _imageMenu->addAction(tr("Zoom &In (25%)"), this, &MainWindow::zoomIn);
  _zoomInAction->setShortcut(QKeySequence::ZoomIn);
  _zoomInAction->setEnabled(false);

  _zoomOutAction = _imageMenu->addAction(tr("Zoom &Out (25%)"), this, &MainWindow::zoomOut);
  _zoomOutAction->setShortcut(QKeySequence::ZoomOut);
  _zoomOutAction->setEnabled(false);

  _setNormalSizeAction = _imageMenu->addAction(tr("&Normal Size"), this, &MainWindow::setNormalSize);
  _setNormalSizeAction->setShortcut(tr("Ctrl+S"));
  _setNormalSizeAction->setEnabled(false);

  _fitToWindowAction = _imageMenu->addAction(tr("&Fit to Window"), this, &MainWindow::fitToWindow);
  _fitToWindowAction->setEnabled(false);
  _fitToWindowAction->setCheckable(true);
  _fitToWindowAction->setShortcut(tr("Ctrl+F"));

  _windowMenu = menuBar()->addMenu(tr("&Window"));

  _helpMenu = menuBar()->addMenu(tr("&Help"));
  _helpMenu->addAction(_aboutAction);
  _helpMenu->addAction(_aboutQtAction);
}

void MainWindow::createToolBars()
{
}

void MainWindow::createStatusBar()
{
}

void MainWindow::createDockWindows()
{
  QDockWidget *dock = new QDockWidget(tr("Exif metadata"), this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    exifView = new QTableView(dock);
    exifView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    exifView->setModel(&_exiv2.exivModel());

    dock->setWidget(exifView);
    addDockWidget(Qt::RightDockWidgetArea, dock);
  _windowMenu->addAction(dock->toggleViewAction());

  dock = new QDockWidget(tr("Map"), this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, dock);
  _windowMenu->addAction(dock->toggleViewAction());

  dock = new QDockWidget(tr("Directory"), this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::LeftDockWidgetArea, dock);;
  _windowMenu->addAction(dock->toggleViewAction());

  dock = new QDockWidget(tr("Comment"), this);
    dock->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
    addDockWidget(Qt::BottomDockWidgetArea, dock);
  _windowMenu->addAction(dock->toggleViewAction());
}

void MainWindow::updateActions()
{
  _zoomInAction       ->setEnabled(!_fitToWindowAction->isChecked());
  _zoomOutAction      ->setEnabled(!_fitToWindowAction->isChecked());
  _setNormalSizeAction->setEnabled(!_fitToWindowAction->isChecked());
}

void MainWindow::setTitle()
{
  QString title = "qexiv";

  if (_imageFilename.length() > 0)
  {
    title.append(" - ");
    title.append(_imageFilename);
  }

  setWindowTitle(title);
}

// -- Image -------------------------------------------------------------------
void MainWindow::openImage(const QString &filename)
{
  QImageReader reader(filename);

#if QT_VERSION >= 0x050500
  reader.setAutoTransform(true);
#endif

  const QImage image = reader.read();

  if (image.isNull())
  {
    QMessageBox::information(this,
                             tr("Error: unable to load image"),
                             tr("Unable to load the image %1: %2").arg(QDir::toNativeSeparators(filename), reader.errorString()));
  }
  else
  {
    _imageFilename = filename;

    setTitle();

    _exiv2.fetch(_imageFilename);

    setImage(image);
  }
}

void MainWindow::setImage(const QImage &image)
{
  _image = image;

  _imageLabel->setPixmap(QPixmap::fromImage(_image));

  _scaleFactor = 1.0;

  _fitToWindowAction->setEnabled(true);

  updateActions();

  if (!_fitToWindowAction->isChecked())
  {
    _imageLabel->adjustSize();
  }
}

void MainWindow::scaleImage(double factor)
{
  _scaleFactor *= factor;

  _imageLabel->resize(_scaleFactor * _imageLabel->pixmap()->size());

  adjustScrollBar(_imageScrollArea->horizontalScrollBar(), factor);
  adjustScrollBar(_imageScrollArea->verticalScrollBar(),   factor);

  _zoomInAction ->setEnabled(_scaleFactor < 5.0);
  _zoomOutAction->setEnabled(_scaleFactor > 0.2);
}

void MainWindow::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
  scrollBar->setValue(int(factor * scrollBar->value() + ((factor - 1) * scrollBar->pageStep()/2)));
}

// -- Slots -------------------------------------------------------------------

void MainWindow::openFile()
{
  if (_imageDialog == 0)
  {
    _imageDialog = new QFileDialog(this, tr("Open image file"));

    _imageDialog->setAcceptMode(QFileDialog::AcceptOpen);
    _imageDialog->setFileMode(QFileDialog::ExistingFile);

#if QT_VERSION >= 0x050000
    const QStringList imageDirectories = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);

    if (imageDirectories.isEmpty())
    {
      _imageDialog->setDirectory(imageDirectories.last());
    }
    else
    {
      _imageDialog->setDirectory(QDir::currentPath());
    }
#else
    _imageDialog->setDirectory(QDir::currentPath());
#endif

#if QT_VERSION >= 0x050200
    QStringList mimeFilters;

    mimeFilters << "image/jpeg";

    _imageDialog->setMimeTypeFilters(mimeFilters);
#else
    QStringList nameFilters;

    nameFilters << "Image files (*.jpg *.JPG *.jpeg *.JPEG)";

    _imageDialog->setNameFilters(nameFilters);
#endif
  }

  if (_imageDialog->exec() == QDialog::Accepted)
  {
    openImage(_imageDialog->selectedFiles().first());
  }

  _imageDialog->close();
}

void MainWindow::zoomIn()
{
  scaleImage(1.25);
}

void MainWindow::zoomOut()
{
  scaleImage(0.8);
}

void MainWindow::setNormalSize()
{
  _imageLabel->adjustSize();

  _scaleFactor = 1.0;
}

void MainWindow::fitToWindow()
{
  bool fitToWindow = _fitToWindowAction->isChecked();

  _imageScrollArea->setWidgetResizable(fitToWindow);

  if (!fitToWindow)
  {
     setNormalSize();
  }

  updateActions();
}

void MainWindow::about()
{
   QMessageBox::about(this, tr("About qexiv"),
            tr("qexiv manipulates the exif metadata in images.\n\n"
               "qexiv Copyright (C) 2016\n\n"
               "This program comes with ABSOLUTELY NO WARRANTY;\n"
               "This is free software, and you are welcome to redistribute it "
               "under certain conditions; see the license for details."));
}

// ----------------------------------------------------------------------------