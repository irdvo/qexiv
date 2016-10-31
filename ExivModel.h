#ifndef _EXIVMODEL_H
#define _EXIVMODEL_H

#include <QAbstractTableModel>
#include <QVariant>
#include <QVector>

class ExivModel : public QAbstractTableModel
{
public:
  ExivModel();
  ~ExivModel();

  // -- QAbstractTableModel implementation ------------------------------------
  int rowCount   (const QModelIndex& parent) const;
  int columnCount(const QModelIndex& parent) const;

  QVariant headerData(int section, Qt::Orientation orientation, int role) const;
  QVariant data(const QModelIndex& index, int role) const;

  // -- Setters ---------------------------------------------------------------
  void clear();

  void add(const QString &key, const QString &type, const QString &length, const QString &value);

  void done();

  // -- Getters ---------------------------------------------------------------
  int length() const { return exivItems_.size(); }

private:

  struct ExivItem
  {
    ExivItem(const QString &key, const QString &type, const QString &length, const QString &value)
    {
      key_    = key;
      type_   = type;
      length_ = length;
      value_  = value;
    }
    ExivItem()
    {
    }

    QString key_;
    QString type_;
    QString length_;
    QString value_;
  };

  typedef QVector<ExivItem> ExivItems;

  ExivItems exivItems_;
};

#endif
