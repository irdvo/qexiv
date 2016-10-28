#include "Exiv2.h"

Exiv2::Exiv2()
{

}

int Exiv2::rowCount(const QModelIndex&) const
{
  return 0;
}

int Exiv2::columnCount(const QModelIndex&) const
{
  return 2;
}

QVariant Exiv2::data(const QModelIndex& index, int role) const
{
  return QVariant::Invalid;
}

QVariant Exiv2::headerData(int section, Qt::Orientation orientation, int role) const
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
