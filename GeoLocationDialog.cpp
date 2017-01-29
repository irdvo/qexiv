#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "GeoLocationDialog.h"


GeoLocationDialog::GeoLocationDialog(QWidget *parent) :
  QDialog(parent)
{
  setWindowTitle(tr("Geolocation"));

  QVBoxLayout *vbox = new QVBoxLayout(this);

    QGridLayout *grid = new QGridLayout;

      grid->addWidget(new QLabel(tr("GPX File:")), 0, 0);

      _filenameEdit = new QLineEdit;
      _filenameEdit->setReadOnly(true);
      grid->addWidget(_filenameEdit, 0, 1, 1, 4);

      _browseButton = new QPushButton(tr("&Browse..."));
      connect(_browseButton, SIGNAL(clicked()), this, SLOT(browse()));
      grid->addWidget(_browseButton, 0, 5);

      grid->addWidget(new QLabel(tr("Tracks:")), 1, 0);
      _tracksEdit = new QLineEdit;
      _tracksEdit->setEnabled(false);
      _tracksEdit->setAlignment(Qt::AlignRight);
      grid->addWidget(_tracksEdit, 1, 1);

      grid->addWidget(new QLabel(tr("Segments:")), 1, 2);
      _segmentsEdit = new QLineEdit;
      _segmentsEdit->setEnabled(false);
      _segmentsEdit->setAlignment(Qt::AlignRight);
      grid->addWidget(_segmentsEdit, 1, 3);

      grid->addWidget(new QLabel(tr("Points:")), 1, 4);
      _pointsEdit = new QLineEdit;
      _pointsEdit->setEnabled(false);
      _pointsEdit->setAlignment(Qt::AlignRight);
      grid->addWidget(_pointsEdit, 1, 5);

    vbox->addLayout(grid);

    _buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    _buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

    vbox->addWidget(_buttonBox);

    connect(_buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
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
      int segments = 0;

      parseXML(xmlFile, tracks, segments, _points);

      _tracksEdit  ->setText(tr("%1").arg(tracks));
      _segmentsEdit->setText(tr("%1").arg(segments));
      _pointsEdit  ->setText(tr("%1").arg(_points.size()));

      _buttonBox->button(QDialogButtonBox::Ok)->setEnabled(_points.size() > 0);
    }
  }
}

void GeoLocationDialog::parseXML(QFile &xmlFile, int &tracks, int &segments, QVector<TrkPt> &points)
{
  QXmlStreamReader xml;

  xml.setDevice(&xmlFile);

  tracks   = 0;
  segments = 0;

  points.clear();

  xml.readNext();
  while (!xml.atEnd())
  {
    if ((xml.isStartElement()) && (xml.name().compare("trk", Qt::CaseInsensitive) == 0))
    {
      parseTrk(xml, segments, points);
      tracks++;
    }

    xml.readNext();
  }
}

void GeoLocationDialog::parseTrk(QXmlStreamReader &xml, int &segments, QVector<TrkPt> &points)
{
  xml.readNext();
  while ((!xml.isEndElement()) || (xml.name().compare("trk", Qt::CaseInsensitive) != 0))
  {
    if ((xml.isStartElement()) && (xml.name().compare("trkseg", Qt::CaseInsensitive) == 0))
    {
      parseTrkSeg(xml, points);
      segments++;
    }

    xml.readNext();
  }
}

void GeoLocationDialog::parseTrkSeg(QXmlStreamReader &xml, QVector<TrkPt> &points)
{
  xml.readNext();
  while ((!xml.isEndElement()) || (xml.name().compare("trkseg", Qt::CaseInsensitive) != 0))
  {
    if ((xml.isStartElement()) && (xml.name().compare("trkpt", Qt::CaseInsensitive) == 0))
    {
      TrkPt trkPt;
      bool  valid = true;

      if (xml.attributes().hasAttribute("lat"))
      {
        trkPt.latitude = xml.attributes().value("lat").toDouble();
      }
      else
      {
        valid = false;
      }

      if (xml.attributes().hasAttribute("lon"))
      {
        trkPt.longitude = xml.attributes().value("lon").toDouble();
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

  while ((!xml.isEndElement()) || (xml.name().compare("trkpt", Qt::CaseSensitive) != 0))
  {
    if ((xml.isStartElement()) && (xml.name().compare("time", Qt::CaseSensitive) == 0))
    {
      QDateTime dateTime = QDateTime::fromString(xml.text().toString(), Qt::ISODate);

#if QT_VERSION >= 0x050800
      secsToEpoch = dateTime.toSecsSinceEpoch();
#else
      secsToEpoch = dateTime.toMSecsSinceEpoch() / 1000;
#endif
      valid = true;
    }

    xml.readNext();
  }

  return valid;
}

// ============================================================================
