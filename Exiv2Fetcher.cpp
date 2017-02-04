#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif
#include <cmath>

#include "Exiv2Fetcher.h"


Exiv2Fetcher::Exiv2Fetcher(QObject *parent) :
  QProcess(parent)
{
  /// todo: better error reporting
  connect(this, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(done(int, QProcess::ExitStatus)));
  connect(this, SIGNAL(readyReadStandardError()),            this, SLOT(reportError()));
  connect(this, SIGNAL(readyReadStandardOutput()),           this, SLOT(reportData()));
}

Exiv2Fetcher::~Exiv2Fetcher()
{

}

// -- Update the exif metadata for the image -----------------------------------
bool Exiv2Fetcher::fetchDateTime(const QString &imageFilename)
{
  if (state() != QProcess::NotRunning)
  {
    return false;
  }

  QStringList parameters;

  parameters << "print" << "-Pt" << "-gExif.Image.DateTime" << imageFilename;

  start("exiv2", parameters);

  return true;
}

// -- Slots -------------------------------------------------------------------
void Exiv2Fetcher::reportError()
{
  QByteArray buffer = readAllStandardError();

  QMessageBox::critical(0, "exiv2 fetch error", buffer);
}

void Exiv2Fetcher::reportData()
{
  emit fetched(QString(readAllStandardOutput()));
}

void Exiv2Fetcher::done(int, ExitStatus)
{
  emit fetched();
}

// ----------------------------------------------------------------------------
