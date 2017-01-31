#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "MainWindow.h"
#include "GeoLocationDialog.h"


MainWindow::MainWindow(int argc, char *argv[]) :
  _imageFilename(""),
  _imagePath(QDir::currentPath()),
  _scaleFactor(1.0),
  _exiv2ModelFetcher(this),
  _exiv2Updater(this)
{
  QCoreApplication::setOrganizationName("qexiv");
  QCoreApplication::setApplicationName("qexiv");

  connect(&_exiv2ModelFetcher, SIGNAL(fetched()), this, SLOT(exifFetched()));
  connect(&_exiv2Updater,      SIGNAL(updated()), this, SLOT(imageUpdated()));

  if (argc > 1)
  {
    if (QFileInfo(argv[1]).isDir())
    {
      _imagePath = argv[1];
    }
  }

  createCentralWidget();
  createActions();
  createMenus();
  createToolBars();
  createStatusBar();
  createMetadataDock();
  createDirectoryDock();

  restoreSettings();

  setTitle();
  updateActions();
}

MainWindow::~MainWindow()
{

}

// == GUI =====================================================================
void MainWindow::createCentralWidget()
{
  QVBoxLayout *vbox = new QVBoxLayout;

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

    vbox->addWidget(_imageScrollArea);

      QGridLayout *grid = new QGridLayout;

      grid->addWidget(new QLabel(tr("Image Description:")), 0, 0);

      _imageDescription = new QLineEdit;
      connect(_imageDescription, SIGNAL(returnPressed()), this, SLOT(updateDescription()));
      grid->addWidget(_imageDescription, 0, 1);

      _setButton = new QPushButton(tr("Set"));
      connect(_setButton, SIGNAL(clicked()), this, SLOT(updateDescription()));
      grid->addWidget(_setButton, 0, 2);

      _nextButton = new QPushButton(tr(">>"));
      connect(_nextButton, SIGNAL(clicked()), this, SLOT(selectNextImage()));
      grid->addWidget(_nextButton, 0, 3);

      grid->addWidget(new QLabel(tr("Latitude:")), 1, 0);

      _latitudeEdit = new QLineEdit;
      connect(_latitudeEdit, SIGNAL(returnPressed()), this, SLOT(updateLatitude()));
      grid->addWidget(_latitudeEdit, 1, 1);

      _latitudeButton = new QPushButton(tr("Set"));
      connect(_latitudeButton, SIGNAL(clicked()), this, SLOT(updateLatitude()));
      grid->addWidget(_latitudeButton, 1, 2);

      grid->addWidget(new QLabel(tr("Longitude:")), 2, 0);

      _longitudeEdit = new QLineEdit;
      connect(_longitudeEdit, SIGNAL(returnPressed()), this, SLOT(updateLongitude()));
      grid->addWidget(_longitudeEdit, 2, 1);

      _longitudeButton = new QPushButton(tr("Set"));
      connect(_longitudeButton, SIGNAL(clicked()), this, SLOT(updateLongitude()));
      grid->addWidget(_longitudeButton, 2, 2);


    vbox->addLayout(grid);

  QWidget *widget = new QWidget(this);
  widget->setLayout(vbox);

  setCentralWidget(widget);
}

void MainWindow::createActions()
{
  QCommonStyle *style = new QCommonStyle;

  QIcon openDirectoryIcon = style->standardIcon(QStyle::SP_DirOpenIcon);
  _openDirectoryAction = new QAction(openDirectoryIcon, tr("&Open Directory..."), this);
  _openDirectoryAction->setStatusTip(tr("Open a directory with image files"));
  connect(_openDirectoryAction, SIGNAL(triggered()), this, SLOT(openDirectory()));

  QIcon parentDirectoryIcon = style->standardIcon(QStyle::SP_FileDialogToParent);
  _parentDirectoryAction = new QAction(parentDirectoryIcon, tr("&Goto Parent Directory"), this);
  _parentDirectoryAction->setStatusTip(tr("Goto the parent directory"));
  connect(_parentDirectoryAction, SIGNAL(triggered()), this, SLOT(parentDirectory()));

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
  _fitToWindowAction->setShortcut(tr("Ctrl+W"));
  _fitToWindowAction->setEnabled(false);
  connect(_fitToWindowAction, SIGNAL(triggered()), this, SLOT(fitToWindow()));

  QIcon showMapIcon = QIcon(":/icons/marker.png");
  _showMapAction = new QAction(showMapIcon, tr("Show location on &Map"), this);
  _showMapAction->setShortcut(tr("Ctrl+M"));
  _showMapAction->setEnabled(false);
  connect(_showMapAction, SIGNAL(triggered()), this, SLOT(showMap()));

  QIcon prevImageIcon = style->standardIcon(QStyle::SP_ArrowBack);
  _prevImageAction = new QAction(prevImageIcon, tr("Show prevous image"), this);
  connect(_prevImageAction, SIGNAL(triggered()), this, SLOT(selectPrevImage()));

  QIcon nextImageIcon = style->standardIcon(QStyle::SP_ArrowForward);
  _nextImageAction = new QAction(nextImageIcon, tr("Show next image"), this);
  connect(_nextImageAction, SIGNAL(triggered()), this, SLOT(selectNextImage()));

  _geoLocateAction = new QAction(tr("GeoLocation..."), this);
  connect(_geoLocateAction, SIGNAL(triggered()), this, SLOT(geoLocate()));

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
  _fileMenu->addAction(_geoLocateAction);
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
  _toolBar = addToolBar("");
  _toolBar->setObjectName("ToolBar");
  _toolBar->addAction(_openDirectoryAction);
  _toolBar->addAction(_parentDirectoryAction);
  _toolBar->addSeparator();
  _toolBar->addAction(_prevImageAction);
  _toolBar->addAction(_nextImageAction);
  _toolBar->addSeparator();
  _toolBar->addAction(_showMapAction);
}

void MainWindow::createStatusBar()
{
  statusBar()->showMessage(tr("Ready"));
}

void MainWindow::createMetadataDock()
{
  QDockWidget *dock = new QDockWidget(tr("Metadata"), this);
    dock->setObjectName("Metadata");
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    _exifView = new QTableView(dock);
#if QT_VERSION >= 0x050000
    _exifView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#else
    _exifView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
#endif
    _exifView->setModel(&_exiv2ModelFetcher.exivModel());

    dock->setWidget(_exifView);
    addDockWidget(Qt::RightDockWidgetArea, dock);
  _windowMenu->addAction(dock->toggleViewAction());
}

void MainWindow::createDirectoryDock()
{
  _directoryDock = new QDockWidget(tr("Images: %1").arg(QDir::currentPath()), this);
    _directoryDock->setObjectName("Images");
    _directoryDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

     _fileSystemModel = new QFileSystemModel(this);
     //_fileSystemModel->setFilter(QDir::AllDirs | QDir::Files | QDir::NoDot);
     _fileSystemModel->setFilter(QDir::Files | QDir::AllDirs | QDir::NoDotAndDotDot);

     QStringList fileSystemFilters; fileSystemFilters << "*.jpg" << "*.JPG" << "*.jpeg" << "*.JPEG";

     _fileSystemModel->setNameFilters(fileSystemFilters);
     _fileSystemModel->setNameFilterDisables(false);
     _fileSystemModel->setRootPath(_imagePath);
     connect(_fileSystemModel, SIGNAL(directoryLoaded(const QString&)), this, SLOT(directoryLoaded(const QString&)));

     _directoryView = new QListView(_directoryDock);
    _directoryView->setModel(_fileSystemModel);
    _directoryView->setRootIndex(_fileSystemModel->index(_imagePath));
    connect(_directoryView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(selectInDirectory(const QModelIndex&)));

    _directoryDock->setWidget(_directoryView);
    addDockWidget(Qt::LeftDockWidgetArea, _directoryDock);;
  _windowMenu->addAction(_directoryDock->toggleViewAction());
}

void MainWindow::updateActions()
{
  _zoomInAction       ->setEnabled(!_fitToWindowAction->isChecked());
  _zoomOutAction      ->setEnabled(!_fitToWindowAction->isChecked());
  _setNormalSizeAction->setEnabled(!_fitToWindowAction->isChecked());
  _setFullSizeAction  ->setEnabled(!_fitToWindowAction->isChecked());
  _showMapAction      ->setEnabled(!_imageFilename.isEmpty());
  _setButton          ->setEnabled(!_imageFilename.isEmpty());
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

// == Image ===================================================================
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

    _exiv2ModelFetcher.fetch(_imageFilename);

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


// == Slots ===================================================================
void MainWindow::deselectDirectorySelections()
{
  QModelIndexList indexes =  _directoryView->selectionModel()->selectedIndexes();

  for (QModelIndexList::iterator index = indexes.begin(); index != indexes.end(); ++index)
  {
    _directoryView->selectionModel()->select(*index, QItemSelectionModel::Deselect);
  }
}

void MainWindow::openDirectory()
{
  QFileDialog directoryDialog(this, tr("Open image directory"), _imagePath);

  directoryDialog.setAcceptMode(QFileDialog::AcceptOpen);
  directoryDialog.setFileMode  (QFileDialog::Directory);
  directoryDialog.setOption    (QFileDialog::ShowDirsOnly);

  if (directoryDialog.exec() == QDialog::Accepted)
  {
    deselectDirectorySelections();

    _imagePath = directoryDialog.selectedFiles().first();

    _directoryDock->setWindowTitle(_imagePath);

    _fileSystemModel->setRootPath(_imagePath);

    _directoryView->setRootIndex(_fileSystemModel->setRootPath(_imagePath));
  }

  directoryDialog.close();
}

void MainWindow::parentDirectory()
{
  QDir directory = _fileSystemModel->rootDirectory();

  if (!directory.isRoot())
  {
    directory.cdUp();

    deselectDirectorySelections();

    _imagePath = directory.absolutePath();

    _directoryDock->setWindowTitle(tr("Images: %1").arg(_imagePath));

    _fileSystemModel->setRootPath(_imagePath);

    _directoryView->setRootIndex(_fileSystemModel->setRootPath(_imagePath));
  }
}

void MainWindow::geoLocate()
{
  GeoLocationDialog *dialog = new GeoLocationDialog(*_fileSystemModel, getFirstIndex(), this);

  if (dialog->exec() == QDialog::Accepted)
  {
    QMessageBox::information(this, tr("Images updated"), tr("There are %1 image(s) updated.").arg(dialog->imagesUpdated()));
  }

  delete dialog;
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

  if (_exiv2ModelFetcher.exivModel().getGPSLocation(latitude, longitude))
  {
    _exiv2ModelFetcher.exivModel().getGPSRef(latitude, longitude);

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

void MainWindow::directoryLoaded(const QString &)
{
  selectFirstImage();
}

void MainWindow::selectInDirectory(const QModelIndex &index)
{
  if (index != QModelIndex())
  {
    deselectDirectorySelections();

    QString filename = _fileSystemModel->fileInfo(index).absoluteFilePath();

    if (_fileSystemModel->fileInfo(index).isDir())
    {
      _directoryDock->setWindowTitle(filename);

      _fileSystemModel->setRootPath(filename);

      _directoryView->setRootIndex(_fileSystemModel->setRootPath(filename));
    }
    else
    {
      // QMessageBox::warning(0, tr("selectFirstImage"), tr("Selection %1").arg(_fileSystemModel->fileInfo(index).absoluteFilePath()));

      _directoryView->selectionModel()->select(index, QItemSelectionModel::Select);
      _directoryView->scrollTo(index);

      openImage(filename);
    }
  }
}

void MainWindow::updateDescription()
{
  // User requested an update of the description of the current image
  _exiv2Updater.update(_imageFilename,  _imageDescription->text());
}

void MainWindow::updateLatitude()
{
  _exiv2Updater.updateGPSLocation(_imageFilename, true, _latitudeEdit->text().toDouble(), false, 0.0);
}

void MainWindow::updateLongitude()
{
  _exiv2Updater.updateGPSLocation(_imageFilename, false, 0.0, true, _longitudeEdit->text().toDouble());
}

void MainWindow::exifFetched()
{
  // Exif data of image is fetched
  if (_exiv2ModelFetcher.exivModel().length() > 0)
  {
    _imageDescription->setText(_exiv2ModelFetcher.exivModel().getImageDescription());

    double latitude;
    double longitude;

    if (_exiv2ModelFetcher.exivModel().getGPSLocation(latitude, longitude))
    {
      _exiv2ModelFetcher.exivModel().getGPSRef(latitude, longitude);

      _latitudeEdit ->setText(tr("%1").arg(latitude));
      _longitudeEdit->setText(tr("%1").arg(longitude));
    }
    else
    {
      _latitudeEdit->setText("");
      _longitudeEdit->setText("");
    }

    statusBar()->showMessage(tr("Exif metadata fetched"));
  }
  else
  {
    statusBar()->showMessage(tr("No exif metadata present in this image"));
  }
}

void MainWindow::imageUpdated()
{
  // Image description is updated, refetch the exif info
  _exiv2ModelFetcher.fetch(_imageFilename);
}

QModelIndex MainWindow::getFirstIndex()
{
  int col = _directoryView->modelColumn();
  int row = 0;

  QModelIndex root = _directoryView->rootIndex();

  QModelIndex sibling = root.child(row, col);

  while ((sibling != QModelIndex()) && (_fileSystemModel->isDir(sibling)))
  {
    sibling = sibling.sibling(++row, col);
  }

  return sibling;
}

void MainWindow::selectFirstImage()
{
  // Strange qt bug: directoryLoaded event is too early -> indices are not yet correct
  QTime waitTime = QTime::currentTime().addMSecs( 500 );
  while (QTime::currentTime() < waitTime)
  {
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
  }

  selectInDirectory(getFirstIndex());
}

void MainWindow::selectPrevImage()
{
  QModelIndexList selectedIndexes =  _directoryView->selectionModel()->selectedIndexes();

  if (selectedIndexes.size() >= 1)
  {
    QModelIndex selected = selectedIndexes.first();

    int row = selected.row();
    int col = selected.column();

    QModelIndex sibling;

    do
    {
       sibling = selected.sibling(--row, col);
    }
    while ((sibling != QModelIndex()) && (_fileSystemModel->isDir(sibling)));

    selectInDirectory(sibling);
  }
  else
  {
    QMessageBox::warning(0, tr("Warning"), tr("No selected image"));
  }
}

void MainWindow::selectNextImage()
{
  QModelIndexList selectedIndexes =  _directoryView->selectionModel()->selectedIndexes();

  if (selectedIndexes.size() >= 1)
  {
    QModelIndex selected = selectedIndexes.first();

    int row = selected.row();
    int col = selected.column();

    QModelIndex sibling;

    do
    {
      sibling = selected.sibling(++row, col);
    }
    while ((sibling != QModelIndex()) && (_fileSystemModel->isDir(sibling)));

    selectInDirectory(sibling);
  }
  else
  {
    QMessageBox::warning(0, tr("Warning"), tr("No selected image"));
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

// == Settings ================================================================
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

// ============================================================================
