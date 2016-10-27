#ifndef _MAINWINDOW_H
#define _MAINWINDOW_H

#include <QMainWindow>
#include <QImage>

class QAction;
class QListWidget;
class QMenu;
class QTextEdit;
class QLabel;
class QScrollArea;
class QFileDialog;
class QScrollBar;

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow();

private slots:
  void openFile();
  void about();

  void zoomIn();
  void zoomOut();
  void setNormalSize();
  void fitToWindow();

private:
  void createActions();
  void createMenus();
  void createToolBars();
  void createStatusBar();
  void createDockWindows();

  void updateActions();

  void adjustScrollBar(QScrollBar *scrollBar, double factor);

  void openImage(const QString &filename);

  void setImage(const QImage &image);
  void scaleImage(double factor);

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
  QAction     *_fitToWindowAction;

  // Image
  QFileDialog *_imageDialog;
  QLabel      *_imageLabel;
  QScrollArea *_imageScrollArea;
  QImage       _image;
  double       _scaleFactor;
};

#endif
