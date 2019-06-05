#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QDoubleValidator>

#include "DescriptionGPSLocationTab.h"


DescriptionGPSLocationTab::DescriptionGPSLocationTab(QWidget *parent) :
  QWidget(parent)
{
  QGridLayout *grid = new QGridLayout;

  grid->addWidget(new QLabel(tr("Image Description:")), 0, 0);

  _imageDescription = new QComboBox;
  _imageDescription->setEditable(true);
  _imageDescription->setInsertPolicy(QComboBox::InsertAtTop);
  connect(_imageDescription, SIGNAL(activated(const QString &)), this, SLOT(setClicked(const QString &)));
  grid->addWidget(_imageDescription, 0, 1);

  _setButton = new QPushButton(tr("Set"));
    connect(_setButton, SIGNAL(clicked()), this, SLOT(setClicked()));
  grid->addWidget(_setButton, 0, 2);

  _nextButton = new QPushButton(tr(">>"));
    connect(_nextButton, SIGNAL(clicked()), this, SLOT(nextClicked()));
  grid->addWidget(_nextButton, 0, 3);

  grid->addWidget(new QLabel(tr("Latitude:")), 1, 0);

  _latitudeEdit = new QLineEdit;
    _latitudeEdit->setValidator(new QDoubleValidator(-90.0, 90.0, 5, this));
    connect(_latitudeEdit, SIGNAL(returnPressed()), this, SLOT(latitudeSet()));
  grid->addWidget(_latitudeEdit, 1, 1);

  _latitudeButton = new QPushButton(tr("Set"));
    connect(_latitudeButton, SIGNAL(clicked()), this, SLOT(latitudeSet()));
  grid->addWidget(_latitudeButton, 1, 2);

  grid->addWidget(new QLabel(tr("Longitude:")), 2, 0);

  _longitudeEdit = new QLineEdit;
    _longitudeEdit->setValidator(new QDoubleValidator(-180.0, 180.0, 5, this));
    connect(_longitudeEdit, SIGNAL(returnPressed()), this, SLOT(longitudeSet()));
  grid->addWidget(_longitudeEdit, 2, 1);

  _longitudeButton = new QPushButton(tr("Set"));
    connect(_longitudeButton, SIGNAL(clicked()), this, SLOT(longitudeSet()));
  grid->addWidget(_longitudeButton, 2, 2);

  setLayout(grid);
}

void DescriptionGPSLocationTab::set(const QString &description, const QString &latitude, const QString &longitude)
{
  if (description == "")
  {
    _imageDescription->clearEditText();
  }
  else
  {
    _imageDescription->setEditText(description);
  }

  _latitudeEdit ->setText(latitude);
  _longitudeEdit->setText(longitude);
}

void DescriptionGPSLocationTab::setClicked(const QString &text)
{
  if (_imageDescription->findText(text, Qt::MatchFixedString|Qt::MatchCaseSensitive) == -1)
  {
    _imageDescription->addItem(text);
  }

  emit descriptionUpdated(text);
}

void DescriptionGPSLocationTab::setClicked()
{
  setClicked(_imageDescription->currentText());
}

void DescriptionGPSLocationTab::nextClicked()
{
  emit nextImageSelected();
}

void DescriptionGPSLocationTab::latitudeSet()
{
  bool ok;

  double value = _latitudeEdit->text().toDouble(&ok);

  if (ok)
  {
    emit latitudeUpdated(value);
  }
}

void DescriptionGPSLocationTab::longitudeSet()
{
  bool ok;

  double value = _longitudeEdit->text().toDouble(&ok);

  if (ok)
  {
    emit longitudeUpdated(value);
  }
}
