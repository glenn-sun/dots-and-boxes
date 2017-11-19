#include "mainwindow.h"
#include <QApplication>
#include <QtWidgets>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow window;
    window.show();
    window.setWindowTitle("Dots and Boxes");
    window.resize(600, 600);
    return a.exec();
}
