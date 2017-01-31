#ifndef _GEOLOCATIONDIALOG_H
#define _GEOLOCATIONDIALOG_H

#include "Exiv2Updater.h"

#include <QDialog>
#include <QModelIndex>

class QLineEdit;
class QPushButton;
class QFile;
class QXmlStreamReader;
class QDialogButtonBox;
class QFileSystemModel;

class GeoLocationDialog : public QDialog
{
  Q_OBJECT

public:
  GeoLocationDialog(QFileSystemModel &fileSystemModel, QModelIndex index, QWidget *parent = 0);
  ~GeoLocationDialog();

private slots:
  void browse();

public:
  struct TrkPt
  {
    double latitude;
    double longitude;
    qint64 secsToEpoch;
  };

  int imagesUpdated() const { return _imagesUpdated; }

private:
  void accept();

  void parseXML(QFile &xmlFile, int &tracks, int &segments, QVector<TrkPt> &_points);
  void parseTrk(QXmlStreamReader &xml, int &segments, QVector<TrkPt> &_points);
  void parseTrkSeg(QXmlStreamReader &xml, QVector<TrkPt> &_points);
  bool parseTime(QXmlStreamReader &xml, qint64 &secsToEpoch);

private:
  QVector<TrkPt>    _points;
  int               _imagesUpdated;

  QLineEdit        *_filenameEdit;
  QPushButton      *_browseButton;
  QLineEdit        *_tracksEdit;
  QLineEdit        *_segmentsEdit;
  QLineEdit        *_pointsEdit;
  QLineEdit        *_secondsOffsetEdit;
  QDialogButtonBox *_buttonBox;

  QFileSystemModel &_fileSystemModel;
  QModelIndex       _index;

  Exiv2Updater      _exiv2Updater;
};

#endif
