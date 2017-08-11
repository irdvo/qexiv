#ifndef OWNER_H
#define OWNER_H

#include <QWidget>

class QLineEdit;
class QPushButton;

class OwnerTab : public QWidget
{
  Q_OBJECT

public:
  OwnerTab(QWidget *parent = 0);
  
  void set(const QString &artist, const QString &copyright, const QString &rating, const QString &processingSoftware);

signals:
  void updated(const QString &artist, const QString &copyright, const QString &rating, const QString &processingSoftware);
  void nextImageSelected();

private slots:
  void setClicked();
  void nextClicked();

private:
  QLineEdit        *_artistEdit;
  QPushButton      *_setButton;
  QPushButton      *_nextButton;
  QLineEdit        *_copyrightEdit;
  QLineEdit        *_ratingEdit;
  QLineEdit        *_processingSoftwareEdit;
};

#endif
