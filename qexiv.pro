VERSION = 0.1

TEMPLATE = app
CONFIG  += qt
equals(QT_MAJOR_VERSION,5) {
  QT      += widgets
}
SOURCES  += main.cpp \
            MainWindow.cpp\
            Exiv2Fetcher.cpp \
            ExivModel.cpp\
            Exiv2Updater.cpp
HEADERS  += MainWindow.h\
            Exiv2Fetcher.h \
            ExivModel.h\
            Exiv2Updater.h
RESOURCES = qexiv.qrc
TARGET    = qexiv

message(Includepath: $$INCLUDEPATH)
message(DestDir:     $$DESTDIR)
message(Libs:        $$LIBS)
message(Qt version:  $$[QT_VERSION])

