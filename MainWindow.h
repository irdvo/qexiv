#ifndef _MAINWINDOW_H
#define _MAINWINDOW_H

#include <QMainWindow>
#include <QImage>

#include "Exiv2Fetcher.h"
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
class QListView;
class QFileSystemModel;
class QDockWidget;
class QPushButton;
class QLineEdit;

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
  void about();

  void zoomIn();
  void zoomOut();
  void setNormalSize();
  void setFullSize();
  void fitToWindow();
  void showMap();
  void selectInDirectory(const QModelIndex &);
  void updateDescription();
  void imageUpdated();
  void exifFetched();
  void directoryLoaded(const QString &);

private:
  void createActions();
  void createMenus();
  void createToolBars();
  void createStatusBar();
  void createDockWindows();

  void setTitle();
  void updateActions();

  void adjustScrollBar(QScrollBar *scrollBar, double factor);

  void openImage(const QString &filename);

  void setImage(const QImage &image);
  void scaleImage(double factor);

  void restoreSettings();
  void saveSettings();

  QMenu       *_fileMenu;
  QMenu       *_imageMenu;
  QMenu       *_windowMenu;
  QMenu       *_helpMenu;

  QAction     *_openDirectoryAction;
  QAction     *_quitAction;
  QAction     *_aboutAction;
  QAction     *_aboutQtAction;

  QAction     *_zoomInAction;
  QAction     *_zoomOutAction;
  QAction     *_setNormalSizeAction;
  QAction     *_setFullSizeAction;
  QAction     *_fitToWindowAction;
  QAction     *_showMapAction;

  // Properties dock
  QTableView  *_exifView;

  // Directory dock
  QDockWidget *_directoryDock;
  QListView   *_directoryView;
  QFileSystemModel *_fileSystemModel;

  // Description dock
  QPushButton *_prevButton;
  QLineEdit   *_imageDescription;
  QPushButton *_setButton;
  QPushButton *_nextButton;

  // Image
  QLabel      *_imageLabel;
  QScrollArea *_imageScrollArea;
  QString      _imageFilename;
  QString      _imagePath;
  QImage       _image;
  double       _scaleFactor;

  // Exiv2
  Exiv2Fetcher _exiv2Fetcher;
  Exiv2Updater _exiv2Updater;
};

#endif
