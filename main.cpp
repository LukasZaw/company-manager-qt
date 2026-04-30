#include "mainwindow.h"

#include "src/database/DatabaseManager.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if (!DatabaseManager::connect()) {
        return -1;
    }

    MainWindow w;
    w.show();
    return a.exec();
}
