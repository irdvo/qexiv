#ifndef _EXIV2_H
#define _EXIV2_H

#include <QAbstractTableModel>
#include <QVariant>

class Exiv2 : public QAbstractTableModel
{
public:
  Exiv2();

  int rowCount   (const QModelIndex& parent) const;
  int columnCount(const QModelIndex& parent) const;

  QVariant headerData(int section, Qt::Orientation orientation, int role) const;
  QVariant data(const QModelIndex& index, int role) const;
};

#endif
