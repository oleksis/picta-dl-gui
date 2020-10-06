#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("Picta-dl_GUI");
    MainWindow w;
    w.setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);
    w.show();

    return a.exec();
}
