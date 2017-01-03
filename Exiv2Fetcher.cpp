#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "Exiv2Fetcher.h"


Exiv2Fetcher::Exiv2Fetcher(QObject *parent) :
  QProcess(parent),
  _exivModel()
{
  /// todo: better error reporting
  connect(this, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(done(int, QProcess::ExitStatus)));
  connect(this, SIGNAL(readyReadStandardError()),            this, SLOT(reportError()));
  connect(this, SIGNAL(readyReadStandardOutput()),           this, SLOT(reportData()));
}

Exiv2Fetcher::~Exiv2Fetcher()
{

}

// -- Fetch the exif metadata for the image -----------------------------------
bool Exiv2Fetcher::fetch(const QString &imageFilename)
{
  bool ok = false;

  if (state() == QProcess::NotRunning)
  {
    _exivModel.clear();

    QStringList parameters;

    parameters << "-pt" << "print" << imageFilename;

    start("exiv2", parameters);
  }

  return ok;
}

// -- Slots -------------------------------------------------------------------
void Exiv2Fetcher::reportError()
{
  QByteArray buffer = readAllStandardError();

  QMessageBox::critical(0, "exiv2 error", buffer);
}

void Exiv2Fetcher::reportData()
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
      _exivModel.add(key, type, length, value);
    }
  }
}


bool Exiv2Fetcher::isSpace(char ch)
{
  return (ch == ' ') || (ch == '\t') || (ch == '\v') || (ch == '\0');
}

void Exiv2Fetcher::skipSpaces(const QByteArray &buffer, int &i)
{
  while ((i < buffer.length()) && (isSpace(buffer.at(i))))
  {
    i++;
  }
}

void Exiv2Fetcher::readTillSpace(const QByteArray &buffer, int &i, QString &result)
{
  result.clear();

  while ((i < buffer.length()) && (!isSpace(buffer.at(i))))
  {
    result.append(buffer.at(i));

    i++;
  }
}

void Exiv2Fetcher::readTillEOL(const QByteArray &buffer, int &i, QString &result)
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

void Exiv2Fetcher::done(int, ExitStatus)
{
  if (_exivModel.length() == 0)
  {
  }

  _exivModel.done();

  emit fetched();
}

// ----------------------------------------------------------------------------
