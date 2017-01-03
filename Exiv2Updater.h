#ifndef _EXIV2UPDATER_H
#define _EXIV2UPDATER_H

#include <QProcess>

class Exiv2Updater : public QProcess
{
  Q_OBJECT

public:
  Exiv2Updater(QObject *parent = 0);
  ~Exiv2Updater();

  // -- Commands --------------------------------------------------------------
  bool update(const QString &imageFilename, const QString &imageDescription);

private slots:
  void reportError();
  void reportData();
  void done(int exitCode, QProcess::ExitStatus exitStatus);

signals:
  void updated();
};

#endif
