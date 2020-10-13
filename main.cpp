#include "mainwindow.h"
#include "RunGuard.h"
#include <QObject>
#include <QApplication>


int main(int argc, char *argv[])
{
    // SingleApplication
    const QString randomKey = "hngiVh8Pe8M5ckidZnxIXTMaR6sg0PNt";
    RunGuard guard(randomKey);
    if(!guard.tryToRun())
        return 0;

    QApplication a(argc, argv);
    a.setApplicationName("Picta-dl_GUI");
    MainWindow w;
    w.setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);
    w.show();

    return a.exec();
}
