#include <QMessageBox>

#include "Exiv2.h"

Exiv2::Exiv2(QObject *parent = 0) :
  QProcess(parent),
  exivModel_()
{
  /// todo: better error reporting
  connect(this, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(done(int, QProcess::ExitStatus)));
  connect(this, SIGNAL(readyReadStandardError()),            this, SLOT(reportError()));
  connect(this, SIGNAL(readyReadStandardOutput()),           this, SLOT(reportData()));
}

Exiv2::~Exiv2()
{

}

// -- Fetch the exif metadata for the image -----------------------------------
bool Exiv2::fetch(const QString &imageFilename)
{
  bool ok = false;

  if (state() == QProcess::NotRunning)
  {
    exivModel_.clear();

    QStringList parameters;

    parameters << "-pt" << "print" << imageFilename;

    start("exiv2", parameters);
  }

  return ok;
}

// -- Slots -------------------------------------------------------------------
void Exiv2::reportError()
{
  QByteArray buffer = readAllStandardError();

  QMessageBox::critical(0, "exiv2 error", buffer);
}

void Exiv2::reportData()
{
  QString key;
  QString type;
  QString length;
  QString value;

  QByteArray buffer = readAllStandardOutput();

  int i = 0;
  while (i < buffer.size())
  {
    skipSpaces(buffer, i);

    readTillSpace(buffer, i, key);

    skipSpaces(buffer, i);

    readTillSpace(buffer, i, type);

    skipSpaces(buffer, i);

    readTillSpace(buffer, i, length);

    skipSpaces(buffer, i);

    readTillEOL(buffer, i, value);

    if (key.length() > 0)
    {
      exivModel_.add(key, type, length, value);
    }
  }
}


bool Exiv2::isSpace(char ch)
{
  return (ch == ' ') || (ch == '\t') || (ch == '\v') || (ch == '\0');
}

void Exiv2::skipSpaces(const QByteArray &buffer, int &i)
{
  while ((i < buffer.length()) && (isSpace(buffer.at(i))))
  {
    i++;
  }
}

void Exiv2::readTillSpace(const QByteArray &buffer, int &i, QString &result)
{
  result.clear();

  while ((i < buffer.length()) && (!isSpace(buffer.at(i))))
  {
    result.append(buffer.at(i));

    i++;
  }
}

void Exiv2::readTillEOL(const QByteArray &buffer, int &i, QString &result)
{
  result.clear();

  while ((i < buffer.length()) && (buffer.at(i) != '\n') && (buffer.at(i) != '\r'))
  {
    result.append(buffer.at(i));

    i++;
  }

  if (buffer.at(i) == '\r')
  {
    i++;
  }

  if (buffer.at(i) == '\n')
  {
    i++;
  }

  result.trimmed();
}

void Exiv2::done(int, ExitStatus)
{
  if (exivModel_.length() == 0)
  {
    QMessageBox::warning(0, tr("Exif metadata"), tr("No exif metadata present in this image"));
  }

  exivModel_.done();
}

// ----------------------------------------------------------------------------
