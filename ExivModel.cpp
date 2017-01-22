#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "ExivModel.h"


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
  return _exivItems.size();
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
      return _exivItems.at(index.row())._key;
    }
    else if (index.column() == 1)
    {
      return _exivItems.at(index.row())._value;
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

  _exivItems.clear();
}

// -- Add one line of data in the stored items --------------------------------
void ExivModel::add(const QString &key, const QString &type, const QString &length, const QString &value)
{
  _exivItems.push_back(ExivItem(key, type, length, value));
}

// -- Indicate the adding of items is done ------------------------------------
void ExivModel::done()
{
  endResetModel();
}

// == Getters =================================================================

// -- Get the GPS location ----------------------------------------------------

bool ExivModel::getGPSLocation(double &latitude, double &longitude)
{
  bool hasLatitude  = false;
  bool hasLongitude = false;

  QString refLatitude  = "";
  QString refLongitude = "";

  for (ExivItems::iterator item = _exivItems.begin(); item != _exivItems.end(); ++item)
  {
    if (item->_key.endsWith("GPSLatitude", Qt::CaseInsensitive))
    {
      hasLatitude = scanGPSLocation(latitude, item->_value);
    }
    if (item->_key.endsWith("GPSLongitude", Qt::CaseInsensitive))
    {
      hasLongitude = scanGPSLocation(longitude, item->_value);
    }

    if (hasLatitude && hasLongitude)
    {
      break;
    }
  }

  return (hasLatitude && hasLongitude);
}

void ExivModel::getGPSRef(double &latitude, double &longitude)
{
  bool hasLatitude  = false;
  bool hasLongitude = false;

  for (ExivItems::iterator item = _exivItems.begin(); item != _exivItems.end(); ++item)
  {
    if (item->_key.endsWith("GPSLatitudeRef", Qt::CaseInsensitive))
    {
      if ((!item->_value.isEmpty()) && ((item->_value[0] == 'S') || (item->_value[0] == 's')))
      {
        latitude = -latitude;
      }

      hasLatitude = true;
    }

    if (item->_key.endsWith("GPSLongitudeRef", Qt::CaseInsensitive))
    {
      if ((!item->_value.isEmpty()) && ((item->_value[0] == 'W') || (item->_value[0] == 'w')))
      {
        longitude = -longitude;
      }

      hasLongitude = true;
    }

    if (hasLatitude && hasLongitude)
    {
      break;
    }
  }
}

QString ExivModel::getImageDescription()
{
  for (ExivItems::iterator item = _exivItems.begin(); item != _exivItems.end(); ++item)
  {
    if (item->_key.endsWith("ImageDescription", Qt::CaseInsensitive))
    {
      return item->_value;
    }
  }

  return "";
}

bool ExivModel::scanGPSLocation(double &location, const QString &value)
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

  location = degrees.toDouble() + minutes.toDouble() / 60.0 + seconds.toDouble() / 3600.0;

  return (!degrees.isEmpty()) && (!minutes.isEmpty()) && (!seconds.isEmpty());
}

// ----------------------------------------------------------------------------
