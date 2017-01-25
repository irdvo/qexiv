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

  QVBoxLayout *vbox = new QVBoxLayout;

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    vbox->addWidget(buttonBox);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  setLayout(vbox);
}

GeoLocationDialog::~GeoLocationDialog()
{

}

// ============================================================================
