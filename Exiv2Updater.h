#ifndef EXIV2UPDATER_H
#define EXIV2UPDATER_H

#include <QProcess>

class Exiv2Updater : public QProcess
{
  Q_OBJECT

public:
  Exiv2Updater(QObject *parent = 0);
  ~Exiv2Updater();

  // -- Commands --------------------------------------------------------------
  bool update(const QString &imageFilename, const QString &imageDescription);

  bool updateGPSLocation (const QString &imageFilename, bool doLatitude, double latitude, bool doLongitude, double longitude);

  bool updateOwner(const QString &imageFilename, const QString &artist, const QString &copyright, const QString &rating, const QString &processingSoftware);

private slots:
  void reportError();
  void reportData();
  void done(int exitCode, QProcess::ExitStatus exitStatus);

signals:
  void updated();
  void failed(const QString message);

private:
  QString position2Degrees(double position);
};

#endif
