#ifndef _GEOLOCATIONDIALOG_H
#define _GEOLOCATIONDIALOG_H

#include "Exiv2Fetcher.h"
#include "Exiv2Updater.h"

#include <QDialog>
#include <QDateTime>

class QLineEdit;
class QPushButton;
class QFile;
class QXmlStreamReader;
class QDialogButtonBox;

class GeoLocationDialog : public QDialog
{
  Q_OBJECT

public:
  GeoLocationDialog(const QVector<QString> &imageFilenames, QWidget *parent = 0);
  ~GeoLocationDialog();

private slots:
  void browse();
  void doGeoLocate();
  void dateTimeFetched(const QString string);
  void dateTimeFetched();
  void doNextImage();

public:
  struct TrkPt
  {
    double latitude;
    double longitude;
    qint64 secsToEpoch;
  };

  int imagesUpdated() const { return _imagesUpdated; }

private:
  void parseXML(QFile &xmlFile, int &tracks, QVector<TrkPt> &_points);
  void parseTrk(QXmlStreamReader &xml, QVector<TrkPt> &_points);
  void parseTrkSeg(QXmlStreamReader &xml, QVector<TrkPt> &_points);
  bool parseTime(QXmlStreamReader &xml, qint64 &secsToEpoch);
  bool lookupGPSLocation(double &latitude, double &longitude);

private:
  const
  QVector<QString>          &_imageFilenames;
  QVector<QString>
    ::const_iterator         _imageFilename;

  QVector<TrkPt>             _points;
  int                        _imagesUpdated;

  QLineEdit                 *_filenameEdit;
  QPushButton               *_browseButton;
  QLineEdit                 *_tracksEdit;
  QLineEdit                 *_imagesEdit;
  QLineEdit                 *_pointsEdit;
  QLineEdit                 *_secondsOffsetEdit;
  QDialogButtonBox          *_buttonBox;

  Exiv2Fetcher               _exiv2Fetcher;
  Exiv2Updater               _exiv2Updater;

  QDateTime                  _imageDateTime;
};

#endif
