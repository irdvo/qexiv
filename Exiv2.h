#ifndef _EXIV2_H
#define _EXIV2_H

#include <QAbstractTableModel>
#include <QVariant>
#include <QProcess>

#include "ExivModel.h"

class Exiv2 : public QProcess // QAbstractTableModel
{
  Q_OBJECT

public:
  Exiv2(QObject *parent);
  ~Exiv2();

  // -- Model -----------------------------------------------------------------
  ExivModel &exivModel() { return exivModel_; }

  // -- Commands --------------------------------------------------------------
  bool fetch(const QString &imageFilename);

private slots:
  void reportError();
  void reportData();
  void done(int exitCode, QProcess::ExitStatus exitStatus);

private:
  void skipSpaces(const QByteArray &buffer, int &i);
  void readTillSpace(const QByteArray &buffer, int &i, QString &result);
  void readTillEOL(const QByteArray &buffer, int &i, QString &result);

  static
  bool isSpace(char ch);

  ExivModel exivModel_;
};

#endif
