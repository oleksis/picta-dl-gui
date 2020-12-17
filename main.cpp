#include "mainwindow.h"
#include "RunGuard.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    // SingleApplication
    const QString randomKey = "hngiVh8Pe8M5ckidZnxIXTMaR6sg0PNt";
    RunGuard guard(randomKey);
    if (!guard.tryToRun())
        return 0;

    QApplication a(argc, argv);
    a.setApplicationName("Picta-dl_GUI");

#ifdef Q_OS_WIN
    QSettings themeSettings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", QSettings::NativeFormat);
    if (themeSettings.value("AppsUseLightTheme") == 0)
    {
        a.setStyle(QStyleFactory::create("Fusion"));

        QPalette darkPalette;
        QColor darkColor = QColor(45, 45, 45);
        QColor disabledColor = QColor(127, 127, 127);

        darkPalette.setColor(QPalette::Window, darkColor);
        darkPalette.setColor(QPalette::WindowText, Qt::white);
        darkPalette.setColor(QPalette::Base, QColor(18, 18, 18));
        darkPalette.setColor(QPalette::AlternateBase, darkColor);
        darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
        darkPalette.setColor(QPalette::ToolTipText, Qt::white);
        darkPalette.setColor(QPalette::Text, Qt::white);
        darkPalette.setColor(QPalette::Disabled, QPalette::Text, disabledColor);
        darkPalette.setColor(QPalette::Button, darkColor);
        darkPalette.setColor(QPalette::ButtonText, Qt::white);
        darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, disabledColor);
        darkPalette.setColor(QPalette::BrightText, Qt::red);
        darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
        darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
        darkPalette.setColor(QPalette::HighlightedText, Qt::black);
        darkPalette.setColor(QPalette::Disabled, QPalette::HighlightedText, disabledColor);

        a.setPalette(darkPalette);
        a.setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }");
    }
#endif

    // Register QSettings
    QSettings::setDefaultFormat(QSettings::IniFormat);
    MainWindow w;
    w.show();

    return a.exec();
}
