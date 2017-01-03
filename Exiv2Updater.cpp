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

// -- Fetch the exif metadata for the image -----------------------------------
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
