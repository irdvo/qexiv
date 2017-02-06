#ifndef _EXIV2MODELFETCHER_H
#define _EXIV2MODELFETCHER_H

#include <QProcess>

#include "ExivModel.h"

class Exiv2ModelFetcher : public QProcess
{
  Q_OBJECT

public:
  Exiv2ModelFetcher(QObject *parent = 0);
  ~Exiv2ModelFetcher();

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
  void failed(const QString message);

private:
  void skipSpaces(const QByteArray &buffer, int &i);
  void readTillSpace(const QByteArray &buffer, int &i, QString &result);
  void readTillEOL(const QByteArray &buffer, int &i, QString &result);

  static
  bool isSpace(char ch);

  ExivModel _exivModel;
};

#endif
