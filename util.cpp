#include "util.h"

QString shortFilename(const QString &filename)
{
  QString noPath;

  for (QString::const_iterator ch = filename.begin(); ch != filename.end(); ++ch)
  {
    if ((*ch == '/') || (*ch == ':'))
    {
      noPath.clear();
    }
    else
    {
      noPath.append(*ch);
    }
  }

  return noPath;
}
