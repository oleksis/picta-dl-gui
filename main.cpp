#include "RunGuard.h"
#include "mainwindow.h"
#include <QApplication>
#include <QProcessEnvironment>

int main(int argc, char *argv[])
{
    // SingleApplication
    const QString randomKey = "hngiVh8Pe8M5ckidZnxIXTMaR6sg0PNt";
    RunGuard guard(randomKey);

    if (!guard.tryToRun())
        return 0;

    QApplication a(argc, argv);
    a.setApplicationName("Picta-dl_GUI");
    bool isDark = false;
    QString PDLG_DARK = QProcessEnvironment::systemEnvironment().value(QString("PDLG_DARK"), QString("0"));
    isDark = (PDLG_DARK == "0" ? false : true);

    if (!isDark)
    {
#ifdef Q_OS_WIN
        QSettings themeSettings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                                QSettings::NativeFormat);
        isDark = themeSettings.value("AppsUseLightTheme") == 0;
#else
        QPalette aPalette = a.palette();
        isDark = aPalette.color(QPalette::Active, QPalette::WindowText).lightness() > 128;
#endif
    }

    QFile fileSheet(isDark ? ":/qdarkstyle/dark/style.qss" : ":/qdarkstyle/light/style.qss");
    fileSheet.open(QFile::ReadOnly);
    QString styleSheet = QString::fromUtf8(fileSheet.readAll());
    a.setStyleSheet(styleSheet);
    fileSheet.close();
    // Register QSettings
    QSettings::setDefaultFormat(QSettings::IniFormat);
    MainWindow w(nullptr, isDark);
    w.show();
    return a.exec();
}
