#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "GeoLocationDialog.h"


GeoLocationDialog::GeoLocationDialog(QWidget *parent) :
  QDialog(parent)
{
  setWindowTitle(tr("Geolocation"));

  QVBoxLayout *vbox = new QVBoxLayout(this);

    QGridLayout *grid = new QGridLayout;

      grid->addWidget(new QLabel(tr("GPX File:")), 0, 0);

      _filenameEdit = new QLineEdit;
      _filenameEdit->setReadOnly(true);
      grid->addWidget(_filenameEdit, 0, 1);

      _browseButton = new QPushButton(tr("&Browse..."));
      connect(_browseButton, SIGNAL(clicked()), this, SLOT(browse()));
      grid->addWidget(_browseButton, 0, 2);

      grid->addWidget(new QLabel(tr("Tracks:")), 1, 0);
      _tracksEdit = new QLineEdit;
      _tracksEdit->setEnabled(false);
      grid->addWidget(_tracksEdit, 1, 1, 1, 2);

      grid->addWidget(new QLabel(tr("Segments:")), 2, 0);
      _segmentsEdit = new QLineEdit;
      _segmentsEdit->setEnabled(false);
      grid->addWidget(_segmentsEdit, 2, 1, 1, 2);

      grid->addWidget(new QLabel(tr("Points:")), 3, 0);
      _pointsEdit = new QLineEdit;
      _pointsEdit->setEnabled(false);
      grid->addWidget(_pointsEdit, 3, 1, 1, 2);

    vbox->addLayout(grid);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    vbox->addWidget(buttonBox);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

GeoLocationDialog::~GeoLocationDialog()
{

}


void GeoLocationDialog::browse()
{
  QString filename =  QFileDialog::getOpenFileName(this, tr("Open GPX file"), "", tr("GPX Files (*.gpx)"));

  if (!filename.isNull())
  {
    _filenameEdit->setText(filename);
  }
}

// ============================================================================
