#ifndef _MAINWINDOW_H
#define _MAINWINDOW_H

#include <QMainWindow>
#include <QImage>

#include "Exiv2.h"

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

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(int argc, char *argv[]);
  ~MainWindow();

protected:
  void closeEvent(QCloseEvent *event);

private slots:
  void openFile();
  void about();

  void zoomIn();
  void zoomOut();
  void setNormalSize();
  void setFullSize();
  void fitToWindow();
  void showMap();
  void selectInDirectory(const QModelIndex &);

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

  QAction     *_openFileAction;
  QAction     *_quitAction;
  QAction     *_aboutAction;
  QAction     *_aboutQtAction;

  QAction     *_zoomInAction;
  QAction     *_zoomOutAction;
  QAction     *_setNormalSizeAction;
  QAction     *_setFullSizeAction;
  QAction     *_fitToWindowAction;
  QAction     *_showMapAction;

  QTableView *exifView;

  QDockWidget *_directoryDock;
  QListView   *_directoryView;
  QFileSystemModel *_fileSystemModel;

  // Image
  QFileDialog *_imageDialog;
  QLabel      *_imageLabel;
  QScrollArea *_imageScrollArea;
  QString      _imageFilename;
  QString      _imagePath;
  QImage       _image;
  double       _scaleFactor;

  // Exiv2
  Exiv2        _exiv2;
};

#endif
