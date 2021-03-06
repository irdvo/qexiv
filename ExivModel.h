#ifndef EXIVMODEL_H
#define EXIVMODEL_H

#include <QAbstractTableModel>
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
  int length() const { return _exivItems.size(); }

  bool getGPSLocation(double &latitude, double &longitude);

  void getGPSRef(double &latitude, double &longitude);

  QString getTagValue(const QString &tag);

private:

  static
  bool scanGPSLocation(double &location, const QString &value);

  struct ExivItem
  {
    ExivItem(const QString &key, const QString &type, const QString &length, const QString &value)
    {
      _key    = key;
      _type   = type;
      _length = length;
      _value  = value;
    }
    ExivItem()
    {
    }

    QString _key;
    QString _type;
    QString _length;
    QString _value;
  };

  typedef QVector<ExivItem> ExivItems;

  ExivItems _exivItems;
};

#endif
