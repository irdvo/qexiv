#ifndef _GEOLOCATIONDIALOG_H
#define _GEOLOCATIONDIALOG_H

#include <QDialog>

class GeoLocationDialog : public QDialog
{
  Q_OBJECT

public:
  GeoLocationDialog(QWidget *parent = 0);
  ~GeoLocationDialog();

};

#endif
