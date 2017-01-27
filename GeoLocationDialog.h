#ifndef _GEOLOCATIONDIALOG_H
#define _GEOLOCATIONDIALOG_H

#include <QDialog>

class QLineEdit;
class QPushButton;

class GeoLocationDialog : public QDialog
{
  Q_OBJECT

public:
  GeoLocationDialog(QWidget *parent = 0);
  ~GeoLocationDialog();

private slots:
  void browse();

private:
  QLineEdit    *_filenameEdit;
  QPushButton  *_browseButton;
  QLineEdit    *_tracksEdit;
  QLineEdit    *_segmentsEdit;
  QLineEdit    *_pointsEdit;
};

#endif
