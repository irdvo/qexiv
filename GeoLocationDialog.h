#ifndef _GEOLOCATIONDIALOG_H
#define _GEOLOCATIONDIALOG_H

#include <QDialog>

class QLineEdit;
class QPushButton;
class QFile;
class QXmlStreamReader;
class QDialogButtonBox;

class GeoLocationDialog : public QDialog
{
  Q_OBJECT

public:
  GeoLocationDialog(QWidget *parent = 0);
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

  QVector<TrkPt> &trkPts() { return _points; }
  int             secondsOffset() { return _secondsOffset; }

private:
  void accept();

  void parseXML(QFile &xmlFile, int &tracks, int &segments, QVector<TrkPt> &_points);
  void parseTrk(QXmlStreamReader &xml, int &segments, QVector<TrkPt> &_points);
  void parseTrkSeg(QXmlStreamReader &xml, QVector<TrkPt> &_points);
  bool parseTime(QXmlStreamReader &xml, qint64 &secsToEpoch);

private:
  QVector<TrkPt>    _points;
  int               _secondsOffset;

  QLineEdit        *_filenameEdit;
  QPushButton      *_browseButton;
  QLineEdit        *_tracksEdit;
  QLineEdit        *_segmentsEdit;
  QLineEdit        *_pointsEdit;
  QLineEdit        *_secondsOffsetEdit;
  QDialogButtonBox *_buttonBox;
};

#endif
