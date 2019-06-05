#ifndef DESCRIPTIONGPSLOCATION_H
#define DESCRIPTIONGPSLOCATION_H

#include <QWidget>

class QLineEdit;
class QPushButton;
class QComboBox;

class DescriptionGPSLocationTab : public QWidget
{
  Q_OBJECT

public:
  DescriptionGPSLocationTab(QWidget *parent = nullptr);
  
  void set(const QString &description, const QString &latitude, const QString &longitude);

signals:
  void descriptionUpdated(const QString &text);
  void latitudeUpdated(double latitude);
  void longitudeUpdated(double longitude);
  void nextImageSelected();

private slots:
  void setClicked();
  void setClicked(const QString &);
  void latitudeSet();
  void longitudeSet();
  void nextClicked();

private:
  QComboBox        *_imageDescription;
  QPushButton      *_setButton;
  QPushButton      *_nextButton;
  QLineEdit        *_latitudeEdit;
  QPushButton      *_latitudeButton;
  QLineEdit        *_longitudeEdit;
  QPushButton      *_longitudeButton;
};

#endif
