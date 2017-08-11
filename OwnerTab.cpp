#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QIntValidator>

#include "OwnerTab.h"


OwnerTab::OwnerTab(QWidget *parent) :
  QWidget(parent)
{
  QGridLayout *grid = new QGridLayout;

  grid->addWidget(new QLabel(tr("Artist:")), 0, 0);

  _artistEdit = new QLineEdit;
  grid->addWidget(_artistEdit, 0, 1);

  _setButton = new QPushButton(tr("Set"));
    connect(_setButton, SIGNAL(clicked()), this, SLOT(setClicked()));
  grid->addWidget(_setButton, 0, 2);

  _nextButton = new QPushButton(tr(">>"));
    connect(_nextButton, SIGNAL(clicked()), this, SLOT(nextClicked()));
  grid->addWidget(_nextButton, 0, 3);

  grid->addWidget(new QLabel(tr("Copyright:")), 1, 0);

  _copyrightEdit = new QLineEdit;
  grid->addWidget(_copyrightEdit, 1, 1);

  grid->addWidget(new QLabel(tr("Rating:")), 2, 0);

  _ratingEdit = new QLineEdit;
    _ratingEdit->setValidator(new QIntValidator(0, 100, this));
  grid->addWidget(_ratingEdit, 2, 1);

  grid->addWidget(new QLabel(tr("ProcessingSoftware:")), 3, 0);

  _processingSoftwareEdit = new QLineEdit;
  grid->addWidget(_processingSoftwareEdit, 3, 1);

  setLayout(grid);
}

void OwnerTab::set(const QString &artist, const QString &copyright, const QString &rating, const QString &processingSoftware)
{
  _artistEdit->setText(artist);
  _copyrightEdit->setText(copyright);
  _ratingEdit->setText(rating);
  _processingSoftwareEdit->setText(processingSoftware);
}

void OwnerTab::setClicked()
{
  emit updated(_artistEdit->text(), _copyrightEdit->text(), _ratingEdit->text(), _processingSoftwareEdit->text());
}

void OwnerTab::nextClicked()
{
  emit nextImageSelected();
}
