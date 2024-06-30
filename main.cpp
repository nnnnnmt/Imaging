#include "mainwindow.h"
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setStyle("fusion");
    QApplication::setFont(QFont("Microsoft YaHei UI"));
    MainWindow w;
    w.setWindowTitle("Imaging");
    w.setWindowIcon(QIcon(":/window/imaging.jpg"));
    w.show();
    return a.exec();
}
