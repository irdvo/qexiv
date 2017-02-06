#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif
#include <cmath>

#include "Exiv2Updater.h"


Exiv2Updater::Exiv2Updater(QObject *parent) :
  QProcess(parent)
{
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

// -- Convert position to degrees
QString Exiv2Updater::position2Degrees(double position)
{
  double degrees = 0.0;
  double minutes = 0.0;
  double seconds = 0.0;

  position = modf(position, &degrees);

  position *= 60.0;

  position = modf(position, &minutes);

  position *= (60.0 * 100.0);

  position = modf(position, &seconds);

  return QString("%1/1 %2/1 %3/100").arg((int) degrees).arg((int) minutes).arg((int) seconds);
}

// -- Update the latitude in the image
bool Exiv2Updater::updateGPSLocation(const QString &imageFilename, bool doLatitude, double latitude, bool doLongitude, double longitude)
{
  if (state() != QProcess::NotRunning)
  {
    return false;
  }

  QStringList parameters;

  parameters << "modify";

  if (doLatitude)
  {
    QString GPSRef = (latitude < 0 ? "S" : "N");

    parameters << "-M" << QString("set Exif.GPSInfo.GPSLatitude %1").arg(position2Degrees(std::abs(latitude)))
               << "-M" << QString("set Exif.GPSInfo.GPSLatitudeRef %1").arg(GPSRef);
  }

  if (doLongitude)
  {
    QString GPSRef = (longitude < 0 ? "W" : "E");

    parameters << "-M" << QString("set Exif.GPSInfo.GPSLongitude %1").arg(position2Degrees(std::abs(longitude)))
               << "-M" << QString("set Exif.GPSInfo.GPSLongitudeRef %1").arg(GPSRef);
  }

  parameters << imageFilename;

  //QMessageBox::information(0, "Update", parameters.join(" "));

  start("exiv2", parameters);

  return true;
}

// -- Slots -------------------------------------------------------------------
void Exiv2Updater::reportError()
{
  emit failed(QString(readAllStandardError()));
}

void Exiv2Updater::reportData()
{
}

void Exiv2Updater::done(int, ExitStatus)
{
  emit updated();
}

// ----------------------------------------------------------------------------
