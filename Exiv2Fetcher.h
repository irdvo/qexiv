#ifndef _EXIV2FETCHER_H
#define _EXIV2FETCHER_H

#include <QProcess>

#include "ExivModel.h"

class Exiv2Fetcher : public QProcess
{
  Q_OBJECT

public:
  Exiv2Fetcher(QObject *parent = 0);
  ~Exiv2Fetcher();

  // -- Model -----------------------------------------------------------------
  ExivModel &exivModel() { return _exivModel; }

  // -- Commands --------------------------------------------------------------
  bool fetch(const QString &imageFilename);

private slots:
  void reportError();
  void reportData();
  void done(int exitCode, QProcess::ExitStatus exitStatus);

signals:
  void fetched();

private:
  void skipSpaces(const QByteArray &buffer, int &i);
  void readTillSpace(const QByteArray &buffer, int &i, QString &result);
  void readTillEOL(const QByteArray &buffer, int &i, QString &result);

  static
  bool isSpace(char ch);

  ExivModel _exivModel;
};

#endif
