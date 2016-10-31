#include "ExivModel.h"

#include <QMessageBox>

ExivModel::ExivModel() :
  QAbstractTableModel()
{
}

ExivModel::~ExivModel()
{
}

// -- QAbstractTableModel implementation --------------------------------------
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

// ----------------------------------------------------------------------------
