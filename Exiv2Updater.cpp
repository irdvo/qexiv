#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "Exiv2Updater.h"


Exiv2Updater::Exiv2Updater(QObject *parent) :
  QProcess(parent)
{
  /// todo: better error reporting
  connect(this, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(done(int, QProcess::ExitStatus)));
  connect(this, SIGNAL(readyReadStandardError()),            this, SLOT(reportError()));
  connect(this, SIGNAL(readyReadStandardOutput()),           this, SLOT(reportData()));
}

Exiv2Updater::~Exiv2Updater()
{

}

// -- Update the exif metadata for the image -----------------------------------
bool Exiv2Updater::update(const QString &imageFilename, const QString &imageDescription)
{
  if (state() != QProcess::NotRunning)
  {
    return false;
  }

  QStringList parameters;

  parameters << "modify" << "-M" << QString("set Exif.Image.ImageDescription %1").arg(imageDescription) << imageFilename;

  start("exiv2", parameters);

  return true;
}

// -- Update the latitude in the image
bool Exiv2Updater::updateLatitude(const QString &imageFilename, double latitude)
{
  if (state() != QProcess::NotRunning)
  {
    return false;
  }

  QString GPSRef = "N";

  if (latitude < 0)
  {
    GPSRef = "S";
    latitude = -latitude;
  }

  double degrees = 0.0;
  double minutes = 0.0;
  double seconds = 0.0;

  latitude = modf(latitude, &degrees);

  latitude *= 60.0;

  latitude = modf(latitude, &minutes);

  latitude *= (60.0 * 100.0);

  latitude = modf(latitude, &seconds);

  QString param = QString("%1/1 %2/1 %3/100").arg((int) degrees).arg((int) minutes).arg((int) seconds);

  QStringList parameters;

  parameters << "modify" << "-M" << QString("set Exif.GPSInfo.GPSLatitude %1").arg(param) << "-M" << QString("set Exif.GPSInfo.GPSLatitudeRef %2").arg(GPSRef) << imageFilename;

  start("exiv2", parameters);

  return true;
}

// -- Update the longitude in the image
bool Exiv2Updater::updateLongitude(const QString &imageFilename, double longitude)
{
  if (state() != QProcess::NotRunning)
  {
    return false;
  }

  QString GPSRef = "E";

  if (longitude < 0)
  {
    GPSRef = "W";
    longitude = -longitude;
  }

  double degrees = 0.0;
  double minutes = 0.0;
  double seconds = 0.0;

  longitude = modf(longitude, &degrees);

  longitude *= 60.0;

  longitude = modf(longitude, &minutes);

  longitude *= (60.0 * 100.0);

  longitude = modf(longitude, &seconds);

  QString param = QString("%1/1 %2/1 %3/100").arg((int) degrees).arg((int) minutes).arg((int) seconds);

  QStringList parameters;

  parameters << "modify" << "-M" << QString("set Exif.GPSInfo.GPSLongitude %1").arg(param) << "-M" << QString("set Exif.GPSInfo.GPSLongitudeRef %1").arg(GPSRef) << imageFilename;

  start("exiv2", parameters);

  return true;
}

// -- Slots -------------------------------------------------------------------
void Exiv2Updater::reportError()
{
  QByteArray buffer = readAllStandardError();

  QMessageBox::critical(0, "exiv2 error", buffer);
}

void Exiv2Updater::reportData()
{
}

void Exiv2Updater::done(int, ExitStatus)
{
  emit updated();
}

// ----------------------------------------------------------------------------
