#include "mainwindow.h"

#include <QApplication>
#include <QFont>
#include <QDir>
#include <QDebug>

// 主函数，应用程序的入口点
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setStyle("Fusion");

    MainWindow w;
    w.showMaximized();

    return a.exec();
}

