#ifndef _EXIV2FETCHER_H
#define _EXIV2FETCHER_H

#include <QProcess>

class Exiv2Fetcher : public QProcess
{
  Q_OBJECT

public:
  Exiv2Fetcher(QObject *parent = 0);
  ~Exiv2Fetcher();

  // -- Commands --------------------------------------------------------------
  bool fetchDateTime(const QString &imageFilename);

private slots:
  void reportError();
  void reportData();
  void done(int exitCode, QProcess::ExitStatus exitStatus);

signals:
  void fetched(const QString data);
  void fetched();
  void failed(const QString message);
};

#endif
