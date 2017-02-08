#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "GeoLocationDialog.h"
#include "util.h"

GeoLocationDialog::GeoLocationDialog(const QVector<QString> &imageFilenames, QPlainTextEdit *messagesBox, QWidget *parent) :
  QDialog(parent),
  _imageFilenames(imageFilenames),
  _messagesBox(messagesBox),
  _imagesUpdated(0)
{
  setWindowTitle(tr("Update the GPS location in all images based on a GPX file"));

  QVBoxLayout *vbox = new QVBoxLayout(this);

    QGridLayout *grid = new QGridLayout;

      grid->addWidget(new QLabel(tr("GPX File:")), 0, 0);

      _filenameEdit = new QLineEdit;
      _filenameEdit->setReadOnly(true);
      grid->addWidget(_filenameEdit, 0, 1, 1, 4);

      _browseButton = new QPushButton(tr("&Browse..."));
      connect(_browseButton, SIGNAL(clicked()), this, SLOT(browse()));
      grid->addWidget(_browseButton, 0, 5);

      grid->addWidget(new QLabel(tr("Images:")), 1, 0);
      _imagesEdit = new QLineEdit;
      _imagesEdit->setEnabled(false);
      _imagesEdit->setAlignment(Qt::AlignRight);
      _imagesEdit->setText(tr("%1").arg(_imageFilenames.size()));
      grid->addWidget(_imagesEdit, 1, 1);

      grid->addWidget(new QLabel(tr("Tracks:")), 1, 2);
      _tracksEdit = new QLineEdit;
      _tracksEdit->setEnabled(false);
      _tracksEdit->setAlignment(Qt::AlignRight);
      grid->addWidget(_tracksEdit, 1, 3);

      grid->addWidget(new QLabel(tr("Points:")), 1, 4);
      _pointsEdit = new QLineEdit;
      _pointsEdit->setEnabled(false);
      _pointsEdit->setAlignment(Qt::AlignRight);
      grid->addWidget(_pointsEdit, 1, 5);

      grid->addWidget(new QLabel(tr("Time offset (seconds):")), 2, 0);
      _secondsOffsetEdit = new QLineEdit;
      _secondsOffsetEdit->setAlignment(Qt::AlignRight);
      _secondsOffsetEdit->setText("0");
      _secondsOffsetEdit->setValidator(new QIntValidator(this));
      grid->addWidget(_secondsOffsetEdit, 2, 1, 1, 5);

    vbox->addLayout(grid);

    _buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    _buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

    vbox->addWidget(_buttonBox);

  connect(_buttonBox,     SIGNAL(accepted()),             this,         SLOT(doGeoLocate()));
  connect(_buttonBox,     SIGNAL(rejected()),             this,         SLOT(reject()));

  connect(&_exiv2Fetcher, SIGNAL(fetched(const QString)), this,         SLOT(dateTimeFetched(const QString)));
  connect(&_exiv2Fetcher, SIGNAL(fetched()),              this,         SLOT(dateTimeFetched()));
  connect(&_exiv2Fetcher, SIGNAL(failed(const QString)),  _messagesBox, SLOT(appendPlainText(const QString)));

  connect(&_exiv2Updater, SIGNAL(updated()),              this,         SLOT(doNextImage()));
  connect(&_exiv2Updater, SIGNAL(failed(const QString)),  _messagesBox, SLOT(appendPlainText(const QString)));
}

GeoLocationDialog::~GeoLocationDialog()
{

}

void GeoLocationDialog::browse()
{
  QString filename =  QFileDialog::getOpenFileName(this, tr("Open GPX file"), "", tr("GPX Files (*.gpx)"));

  if (!filename.isNull())
  {
    _filenameEdit->setText(filename);

    QFile xmlFile(filename);

    if (xmlFile.open(QIODevice::ReadOnly))
    {
      int tracks   = 0;

      parseXML(xmlFile, tracks, _points);

      _tracksEdit  ->setText(tr("%1").arg(tracks));
      _pointsEdit  ->setText(tr("%1").arg(_points.size()));

      _buttonBox->button(QDialogButtonBox::Ok)->setEnabled(_points.size() > 0);
    }
  }
}

void GeoLocationDialog::doGeoLocate()
{
  _imageFilename = _imageFilenames.begin();

  if (_imageFilename != _imageFilenames.end())
  {
    _exiv2Fetcher.fetchDateTime(*_imageFilename);
  }
}

void GeoLocationDialog::dateTimeFetched(const QString string)
{
  // Process the date time
  _imageDateTime = QDateTime::fromString(string.trimmed(), "yyyy:MM:dd HH:mm:ss");
}

void GeoLocationDialog::dateTimeFetched()
{
  double latitude;
  double longitude;

  if ((_imageDateTime.isValid()) && (lookupGPSLocation(latitude, longitude)))
  {
    _messagesBox->appendPlainText(tr("%1: time %2 located on latitude:%3 and longitude:%4").arg(shortFilename(*_imageFilename)).arg(_imageDateTime.toString("yyyy:MM:dd HH:mm:ss")).arg(latitude).arg(longitude));

    _exiv2Updater.updateGPSLocation(*_imageFilename, true, latitude, true, longitude);
    _imagesUpdated++;
  }
  else
  {
    if (_imageDateTime.isValid())
    {
      _messagesBox->appendPlainText(tr("%1: time %2 not located").arg(shortFilename(*_imageFilename)).arg(_imageDateTime.toString("yyyy:MM:dd HH:mm:ss")));
    }
    else
    {
      _messagesBox->appendPlainText(tr("%1: unknown time").arg(shortFilename(*_imageFilename)));
    }

    doNextImage();
  }
}

void GeoLocationDialog::doNextImage()
{
  if (_imageFilename != _imageFilenames.end())
  {
    ++_imageFilename;
  }

  if (_imageFilename != _imageFilenames.end())
  {
    _exiv2Fetcher.fetchDateTime(*_imageFilename);
  }
  else
  {
    QDialog::accept();
  }
}


void GeoLocationDialog::parseXML(QFile &xmlFile, int &tracks, QVector<TrkPt> &points)
{
  QXmlStreamReader xml;

  xml.setDevice(&xmlFile);

  tracks   = 0;

  points.clear();

  xml.readNext();
  while (!xml.atEnd())
  {
    if ((xml.isStartElement()) && (xml.name().compare(QString("trk"), Qt::CaseInsensitive) == 0))
    {
      parseTrk(xml, points);
      tracks++;
    }

    xml.readNext();
  }
}

void GeoLocationDialog::parseTrk(QXmlStreamReader &xml, QVector<TrkPt> &points)
{
  xml.readNext();
  while ((!xml.isEndElement()) || (xml.name().compare(QString("trk"), Qt::CaseInsensitive) != 0))
  {
    if ((xml.isStartElement()) && (xml.name().compare(QString("trkseg"), Qt::CaseInsensitive) == 0))
    {
      parseTrkSeg(xml, points);
    }

    xml.readNext();
  }
}

void GeoLocationDialog::parseTrkSeg(QXmlStreamReader &xml, QVector<TrkPt> &points)
{
  xml.readNext();
  while ((!xml.isEndElement()) || (xml.name().compare(QString("trkseg"), Qt::CaseInsensitive) != 0))
  {
    if ((xml.isStartElement()) && (xml.name().compare(QString("trkpt"), Qt::CaseInsensitive) == 0))
    {
      TrkPt trkPt;
      bool  valid = true;

      if (xml.attributes().hasAttribute("lat"))
      {
        trkPt.latitude = xml.attributes().value("lat").toString().toDouble();
      }
      else
      {
        valid = false;
      }

      if (xml.attributes().hasAttribute("lon"))
      {
        trkPt.longitude = xml.attributes().value("lon").toString().toDouble();
      }
      else
      {
        valid = false;
      }

      if (!parseTime(xml, trkPt.secsToEpoch))
      {
          valid =false;
      }

      if (valid)
      {
        points.append(trkPt);
      }
    }

    xml.readNext();
  }
}

bool GeoLocationDialog::parseTime(QXmlStreamReader &xml, qint64 &secsToEpoch)
{
  bool valid = false;

  while ((!xml.isEndElement()) || (xml.name().compare(QString("trkpt"), Qt::CaseSensitive) != 0))
  {
    if ((xml.isStartElement()) && (xml.name().compare(QString("time"), Qt::CaseSensitive) == 0))
    {
      xml.readNext();

      while ((!xml.isEndElement()) || (xml.name().compare(QString("time"), Qt::CaseSensitive) != 0))
      {
        if (xml.isCharacters())
        {
          QDateTime dateTime = QDateTime::fromString(xml.text().toString(), Qt::ISODate);

          if (dateTime.isValid())
          {
#if QT_VERSION >= 0x050800
            secsToEpoch = dateTime.toSecsSinceEpoch();
#else
            secsToEpoch = dateTime.toMSecsSinceEpoch() / 1000;
#endif
            valid = true;
          }
        }

        xml.readNext();
      }
    }

    xml.readNext();
  }

  return valid;
}

bool GeoLocationDialog::lookupGPSLocation(double &latitude, double &longitude)
{
  if (_points.empty())
  {
    return false;
  }

#if QT_VERSION >= 0x050800
  qint64 imageSecsToEpoch = _imageDateTime.toSecsSinceEpoch();
#else
  qint64 imageSecsToEpoch = _imageDateTime.toMSecsSinceEpoch() / 1000;
#endif

  imageSecsToEpoch += _secondsOffsetEdit->text().toInt();

  if ((imageSecsToEpoch < _points.first().secsToEpoch) || (imageSecsToEpoch > _points.last().secsToEpoch))
  {
    return false;
  }

  TrkPt lastPoint = _points.first();

  for (QVector<TrkPt>::iterator iter = _points.begin(); iter != _points.end(); ++iter)
  {
    if (imageSecsToEpoch == iter->secsToEpoch)
    {
      latitude  = iter->latitude;
      longitude = iter->longitude;

      return true;
    }
    else if (imageSecsToEpoch < iter->secsToEpoch)
    {
      latitude  = lastPoint.latitude  + (iter->latitude  - lastPoint.latitude)  * ((double)(imageSecsToEpoch - lastPoint.secsToEpoch) / (double)(iter->secsToEpoch - lastPoint.secsToEpoch));
      longitude = lastPoint.longitude + (iter->longitude - lastPoint.longitude) * ((double)(imageSecsToEpoch - lastPoint.secsToEpoch) / (double)(iter->secsToEpoch - lastPoint.secsToEpoch));

      return true;
    }
    else
    {
      lastPoint = *iter;
    }
  }

  return false;
}

// ============================================================================
