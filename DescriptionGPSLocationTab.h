#ifndef DESCRIPTIONGPSLOCATION_H
#define DESCRIPTIONGPSLOCATION_H

#include <QWidget>

class QLineEdit;
class QPushButton;

class DescriptionGPSLocationTab : public QWidget
{
  Q_OBJECT

public:
  DescriptionGPSLocationTab(QWidget *parent = 0);
  
  void set(const QString &description, const QString &latitude, const QString &longitude);

signals:
  void descriptionUpdated(const QString &text);
  void latitudeUpdated(double latitude);
  void longitudeUpdated(double longitude);
  void nextImageSelected();

private slots:
  void setClicked();
  void latitudeSet();
  void longitudeSet();
  void nextClicked();

private:
  QLineEdit        *_imageDescription;
  QPushButton      *_setButton;
  QPushButton      *_nextButton;
  QLineEdit        *_latitudeEdit;
  QPushButton      *_latitudeButton;
  QLineEdit        *_longitudeEdit;
  QPushButton      *_longitudeButton;
};

#endif
