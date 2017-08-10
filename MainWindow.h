#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QModelIndex>

#include "Exiv2ModelFetcher.h"
#include "Exiv2Updater.h"

class QAction;
class QListWidget;
class QMenu;
class QTextEdit;
class QLabel;
class QScrollArea;
class QFileDialog;
class QScrollBar;
class QTableView;
class FileView;
class QFileSystemModel;
class QDockWidget;
class QPushButton;
class QLineEdit;
class QPlainTextEdit;
class DescriptionGPSLocationTab;

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(int argc, char *argv[]);
  ~MainWindow();

protected:
  void closeEvent(QCloseEvent *event);

private slots:
  void openDirectory();
  void parentDirectory();
  void about();
  void geoLocate();
  void zoomIn();
  void zoomOut();
  void setNormalSize();
  void setFullSize();
  void fitToWindow();
  void showMap();
  void updateDescription(const QString &);
  void updateLatitude(double);
  void updateLongitude(double);
  void imageUpdated();
  void exifFetched();
  void directoryLoaded(const QString &);
  void rootPathChanged(const QString &);
  void indexSelected(const QModelIndex &);
  void indexActivated(const QModelIndex &);
  void moveToPrevImage();
  void moveToNextImage();

private:
  void createCentralWidget();
  void createActions();
  void createMenus();
  void createToolBars();
  void createMetadataDock();
  void createDirectoryDock();
  void createMessagesDock();

  void setTitle();
  void updateActions();

  void adjustScrollBar(QScrollBar *scrollBar, double factor);

  void setImage(const QString &filename);
  void clearImage();
  bool openImage(const QString &filename, QLabel *label, QSize &size);
  void scaleImage(double factor);

  void restoreSettings();
  void saveSettings();

  void moveToFirstImage();
  void moveToCurrentImage();
  QModelIndex getFirstIndex();
  void moveToIndex(const QModelIndex &);

  void deselectDirectorySelections();


  QMenu            *_fileMenu;
  QMenu            *_imageMenu;
  QMenu            *_windowMenu;
  QMenu            *_helpMenu;
  QToolBar         *_toolBar;

  QAction          *_openDirectoryAction;
  QAction          *_parentDirectoryAction;
  QAction          *_quitAction;
  QAction          *_aboutAction;
  QAction          *_aboutQtAction;

  QAction          *_zoomInAction;
  QAction          *_zoomOutAction;
  QAction          *_setNormalSizeAction;
  QAction          *_setFullSizeAction;
  QAction          *_fitToWindowAction;
  QAction          *_showMapAction;

  QAction          *_prevImageAction;
  QAction          *_nextImageAction;

  QAction          *_geoLocateAction;

  // Properties dock
  QTableView       *_exifView;

  // Directory dock
  QDockWidget      *_directoryDock;
  FileView         *_directoryView;
  QFileSystemModel *_fileSystemModel;

  // Message dock
  QDockWidget      *_messagesDock;
  QPlainTextEdit   *_messagesBox;

  // Description dock
  QTabWidget       *_actionTab;
  DescriptionGPSLocationTab *_descriptionGPSLocationTab;

  // Image
  QLabel           *_imageLabel;
  QScrollArea      *_imageScrollArea;
  QString           _imageFilename;
  QString           _imagePath;
  QSize             _imageSize;
  double            _scaleFactor;

  // Exiv2
  Exiv2ModelFetcher _exiv2ModelFetcher;
  Exiv2Updater      _exiv2Updater;
};

#endif
