#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.setWindowTitle("Magnet");
    w.setWindowIcon(QIcon(":/img/icon512.png"));
    w.show();

    return a.exec();
}
