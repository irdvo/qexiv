#include <QApplication>

#include "MainWindow.h"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow main(argc, argv);

    main.show();

    return app.exec();
}

