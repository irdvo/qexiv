#include "FileView.h"

FileView::FileView(QWidget *parent) :
  QListView(parent)
{
}

void FileView::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
  QListView::currentChanged(current, previous);

  emit indexSelected(current);
}
