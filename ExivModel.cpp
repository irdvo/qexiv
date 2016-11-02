#include "ExivModel.h"

#include <QMessageBox>

ExivModel::ExivModel() :
  QAbstractTableModel()
{
}

ExivModel::~ExivModel()
{
}

// == QAbstractTableModel implementation ======================================
int ExivModel::rowCount(const QModelIndex&) const
{
  return exivItems_.size();
}

int ExivModel::columnCount(const QModelIndex&) const
{
  return 2;
}

QVariant ExivModel::data(const QModelIndex& index, int role) const
{
  if (role == Qt::DisplayRole)
  {
    if (index.column() == 0)
    {
      return exivItems_.at(index.row()).key_;
    }
    else if (index.column() == 1)
    {
      return exivItems_.at(index.row()).value_;
    }
  }
  return QVariant::Invalid;
}

QVariant ExivModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if ((role == Qt::DisplayRole) && (orientation == Qt::Horizontal))
  {
    if (section == 0)
    {
      return tr("Key");
    }
    else if (section == 1)
    {
      return tr("Value");
    }
  }

  return QVariant::Invalid;
}

// == Fill the model ==========================================================

// -- Clear the stored items --------------------------------------------------
void ExivModel::clear()
{
  beginResetModel();

  exivItems_.clear();
}

// -- Add one line of data in the stored items --------------------------------
void ExivModel::add(const QString &key, const QString &type, const QString &length, const QString &value)
{
  exivItems_.push_back(ExivItem(key, type, length, value));
}

// -- Indicate the adding of items is done ------------------------------------
void ExivModel::done()
{
  endResetModel();
}

// == Getters =================================================================

// -- Get the GPS location ----------------------------------------------------

bool ExivModel::getGPSLocation(double *latitude, double *longitude)
{
  bool hasLatitude  = false;
  bool hasLongitude = false;

  for (ExivItems::iterator item = exivItems_.begin(); item != exivItems_.end(); ++item)
  {
    if (item->key_.endsWith("GPSLatitude", Qt::CaseInsensitive))
    {
      hasLatitude = scanGPSLocation(latitude, item->value_);
    }
    if (item->key_.endsWith("GPSLongitude", Qt::CaseInsensitive))
    {
      hasLongitude = scanGPSLocation(longitude, item->value_);
    }

    if (hasLatitude && hasLongitude)
    {
      break;
    }
  }

  return (hasLatitude && hasLongitude);
}

bool ExivModel::scanGPSLocation(double *location, const QString &value)
{
  QString degrees;
  QString minutes;
  QString seconds;

  int i = 0;
  while ((i < value.size()) && (value.at(i).isDigit()))
  {
    degrees.append(value.at(i++));
  }

  while ((i < value.size()) && (!value.at(i).isDigit()))
  {
    i++;
  }

  while ((i < value.size()) && (value.at(i).isDigit()))
  {
    minutes.append(value.at(i++));
  }

  while ((i < value.size()) && (!value.at(i).isDigit()))
  {
    i++;
  }

  while ((i < value.size()) && ((value.at(i).isDigit()) || (value.at(i) == '.')))
  {
    seconds.append(value.at(i++));
  }

  if (location != 0)
  {
    *location = degrees.toDouble() + minutes.toDouble() / 60.0 + seconds.toDouble() / 3600.0; }

  return (!degrees.isEmpty()) && (!minutes.isEmpty()) && (!seconds.isEmpty());
}

// ----------------------------------------------------------------------------
