VERSION = 0.1

TEMPLATE = app
CONFIG  += qt
equals(QT_MAJOR_VERSION,5) {
  QT      += widgets
}
SOURCES += main.cpp \
           MainWindow.cpp\
           Exiv2.cpp \
           ExivModel.cpp
HEADERS += MainWindow.h\
           Exiv2.h \
           ExivModel.h
TARGET   = qexiv

message(Includepath: $$INCLUDEPATH)
message(DestDir:     $$DESTDIR)
message(Libs:        $$LIBS)
message(Qt version:  $$[QT_VERSION])

