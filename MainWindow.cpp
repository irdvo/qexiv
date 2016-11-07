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
#include <QTextStream>
#include <QDesktopServices>
#include <QUrl>
#include <QListView>
#include <QFileSystemModel>
#include <QSettings>

#if QT_VERSION >= 0x050000
#include <QStandardPaths>
#endif

#include "MainWindow.h"


MainWindow::MainWindow(int argc, char *argv[]) :
  _imageFilename(""),
  _imagePath(QDir::currentPath()),
  _exiv2(this)
{
  QCoreApplication::setOrganizationName("qexiv");
  QCoreApplication::setApplicationName("qexiv");

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

  if (argc > 1)
  {
    if (QFileInfo(argv[1]).isDir())
    {
      _imagePath = argv[1];
    }
  }

  createActions();
  createMenus();
  createToolBars();
  createStatusBar();
  createDockWindows();

  restoreSettings();

  setTitle();
}

MainWindow::~MainWindow()
{

}

// -- GUI ---------------------------------------------------------------------
void MainWindow::createActions()
{
  _openDirectoryAction = new QAction(tr("&Open Directory..."), this);
  _openDirectoryAction->setStatusTip(tr("Open a directory with image files"));
  connect(_openDirectoryAction, SIGNAL(triggered()), this, SLOT(openDirectory()));

  _quitAction = new QAction(tr("&Quit"), this);
  _quitAction->setShortcuts(QKeySequence::Quit);
  _quitAction->setStatusTip(tr("Quit the application"));
  connect(_quitAction, SIGNAL(triggered()), this, SLOT(close()));

  _zoomInAction = new QAction(tr("Zoom &In (25%)"), this);
  _zoomInAction->setShortcut(QKeySequence::ZoomIn);
  _zoomInAction->setEnabled(false);
  connect(_zoomInAction, SIGNAL(triggered()), this, SLOT(zoomIn()));

  _zoomOutAction = new QAction(tr("Zoom &Out (25%)"), this);
  _zoomOutAction->setShortcut(QKeySequence::ZoomOut);
  _zoomOutAction->setEnabled(false);
  connect(_zoomOutAction, SIGNAL(triggered()), this, SLOT(zoomOut()));

  _setNormalSizeAction = new QAction(tr("&Normal Size"), this);
  _setNormalSizeAction->setShortcut(tr("Ctrl+S"));
  _setNormalSizeAction->setEnabled(false);
  connect(_setNormalSizeAction, SIGNAL(triggered()), this, SLOT(setNormalSize()));

  _setFullSizeAction = new QAction(tr("&Full Size"), this);
  _setFullSizeAction->setShortcut(tr("Ctrl+F"));
  _setFullSizeAction->setEnabled(false);
  connect(_setFullSizeAction, SIGNAL(triggered()), this, SLOT(setFullSize()));

  _fitToWindowAction = new QAction(tr("&Fit to Window"), this);
  _fitToWindowAction->setCheckable(true);
  _fitToWindowAction->setShortcut(tr("Ctrl+F"));
  _fitToWindowAction->setEnabled(false);
  connect(_fitToWindowAction, SIGNAL(triggered()), this, SLOT(fitToWindow()));

  _showMapAction = new QAction(tr("Show location on &Map"), this);
  _showMapAction->setShortcut(tr("Ctrl+M"));
  _showMapAction->setEnabled(false);
  connect(_showMapAction, SIGNAL(triggered()), this, SLOT(showMap()));

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
  _fileMenu->addAction(_openDirectoryAction);
  _fileMenu->addSeparator();
  _fileMenu->addAction(_quitAction);

  _imageMenu = menuBar()->addMenu(tr("&Image"));

  _imageMenu->addAction(_zoomInAction);
  _imageMenu->addAction(_zoomOutAction);
  _imageMenu->addAction(_setNormalSizeAction);
  _imageMenu->addAction(_setFullSizeAction);
  _imageMenu->addAction(_fitToWindowAction);
  _fileMenu->addSeparator();
  _imageMenu->addAction(_showMapAction);

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
  QDockWidget *dock = new QDockWidget(tr("Metadata"), this);
    dock->setObjectName("Metadata");
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    exifView = new QTableView(dock);
#if QT_VERSION >= 0x050000
    exifView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#else
    exifView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
#endif
    exifView->setModel(&_exiv2.exivModel());

    dock->setWidget(exifView);
    addDockWidget(Qt::RightDockWidgetArea, dock);
  _windowMenu->addAction(dock->toggleViewAction());

  _directoryDock = new QDockWidget(tr("Images: %1").arg(QDir::currentPath()), this);
    _directoryDock->setObjectName("Images");
    _directoryDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

     _fileSystemModel = new QFileSystemModel(this);
     _fileSystemModel->setRootPath(_imagePath);
     _fileSystemModel->setFilter(QDir::AllDirs | QDir::Files | QDir::NoDot);

     QStringList fileSystemFilters; fileSystemFilters << "*.jpg" << "*.JPG" << "*.jpeg" << "*.JPEG";

     _fileSystemModel->setNameFilters(fileSystemFilters);
     _fileSystemModel->setNameFilterDisables(false);

     _directoryView = new QListView(_directoryDock);
    _directoryView->setModel(_fileSystemModel);
    _directoryView->setRootIndex(_fileSystemModel->index(_imagePath));
    connect(_directoryView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(selectInDirectory(const QModelIndex&)));

    _directoryDock->setWidget(_directoryView);
    addDockWidget(Qt::LeftDockWidgetArea, _directoryDock);;
  _windowMenu->addAction(_directoryDock->toggleViewAction());

  dock = new QDockWidget(tr("Comment"), this);
    dock->setObjectName("Comment");
    dock->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
    addDockWidget(Qt::BottomDockWidgetArea, dock);
  _windowMenu->addAction(dock->toggleViewAction());
}

void MainWindow::updateActions()
{
  _zoomInAction       ->setEnabled(!_fitToWindowAction->isChecked());
  _zoomOutAction      ->setEnabled(!_fitToWindowAction->isChecked());
  _setNormalSizeAction->setEnabled(!_fitToWindowAction->isChecked());
  _setFullSizeAction  ->setEnabled(!_fitToWindowAction->isChecked());
  _showMapAction      ->setEnabled(!_imageFilename.isEmpty());
}

void MainWindow::setTitle()
{
  QString title = "qexiv";

  if (_imageFilename.length() > 0)
  {
    title.append(QString(" - %1 (%2%)").arg(_imageFilename).arg((int) (_scaleFactor * 100.0)));
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
    setNormalSize();
  }
}

void MainWindow::scaleImage(double factor)
{
  _scaleFactor *= factor;

  _imageLabel->resize(_scaleFactor * _imageLabel->pixmap()->size());

  adjustScrollBar(_imageScrollArea->horizontalScrollBar(), factor);
  adjustScrollBar(_imageScrollArea->verticalScrollBar(),   factor);

  _zoomInAction ->setEnabled(_scaleFactor < 2.0);
  _zoomOutAction->setEnabled(_scaleFactor > 0.1);

  setTitle();
}

void MainWindow::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
  scrollBar->setValue(int(factor * scrollBar->value() + ((factor - 1) * scrollBar->pageStep()/2)));
}

// -- Slots -------------------------------------------------------------------

void MainWindow::openDirectory()
{
  QFileDialog directoryDialog(this, tr("Open image directory"), _imagePath);

  directoryDialog.setAcceptMode(QFileDialog::AcceptOpen);
  directoryDialog.setFileMode  (QFileDialog::Directory);

  if (directoryDialog.exec() == QDialog::Accepted)
  {
    _imagePath = directoryDialog.selectedFiles().first();

    _directoryView->setRootIndex(_fileSystemModel->setRootPath(_imagePath));
  }

  directoryDialog.close();
}

void MainWindow::zoomIn()
{
  scaleImage(1.25);
}

void MainWindow::zoomOut()
{
  scaleImage(0.75);
}

void MainWindow::setNormalSize()
{
  _scaleFactor = 1.0;

  // Scale the image to fit on screen
  double factor = qMin((double) _imageScrollArea->size().width()  / (double) _image.size().width(),
                       (double) _imageScrollArea->size().height() / (double) _image.size().height());

  scaleImage(factor * 0.95);

}

void MainWindow::setFullSize()
{
  _scaleFactor = 1.0;

  _imageLabel->adjustSize();
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

void MainWindow::showMap()
{
  double latitude;
  double longitude;

  if (_exiv2.exivModel().getGPSLocation(&latitude, &longitude))
  {
    QString url = QString("http://www.openstreetmap.org/?mlat=%1&mlon=%2#map=16/%3/%4")
        .arg(latitude,  0, 'f', 7)
        .arg(longitude, 0, 'f', 7)
        .arg(latitude,  0, 'f', 7)
        .arg(longitude, 0, 'f', 7);

    if (!QDesktopServices::openUrl(QUrl(url)))
    {
      QMessageBox::warning(this, tr("Browser failure"), tr("Your browser failed to show the GPS location of the image."));
    }
  }
  else
  {
    QMessageBox::warning(this, tr("Missing GPS location"), tr("There was no GPS location found in the image."));
  }
}

void MainWindow::selectInDirectory(const QModelIndex &index)
{
  QString filename = _fileSystemModel->fileInfo(index).absoluteFilePath();

  if (_fileSystemModel->fileInfo(index).isDir())
  {
    if (filename != "/..")
    {
      _directoryDock->setWindowTitle(tr("Images: %1").arg(filename));

      _directoryView->setRootIndex(_fileSystemModel->setRootPath(filename));
    }
  }
  else
  {
    openImage(filename);
  }
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

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveSettings();

    QMainWindow::closeEvent(event);
}

// -- Settings ----------------------------------------------------------------

void MainWindow::saveSettings()
{
  QSettings settings;

  settings.setValue("MainWindow/Geometry", saveGeometry());
  settings.setValue("MainWindow/State",    saveState()   );
}

void MainWindow::restoreSettings()
{
  QSettings settings;

  restoreGeometry(settings.value("MainWindow/Geometry").toByteArray());
  restoreState   (settings.value("MainWindow/State"   ).toByteArray());
}

// ----------------------------------------------------------------------------
