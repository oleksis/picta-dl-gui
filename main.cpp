#include "mainwindow.h"
#include "RunGuard.h"
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
        QSettings themeSettings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", QSettings::NativeFormat);
        isDark = themeSettings.value("AppsUseLightTheme") == 0;
#else
        QPalette aPalette = a.palette();
        isDark = aPalette.color(QPalette::Active, QPalette::WindowText).lightness() > 128;
#endif
    }

    if (isDark)
    {
        a.setStyle(QStyleFactory::create("Fusion"));
        QPalette darkPalette;
        QColor darkColor = QColor(10, 10, 10);
        QColor disabledColor = QColor(127, 127, 127);
        darkPalette.setColor(QPalette::Window, darkColor);
        darkPalette.setColor(QPalette::WindowText, Qt::white);
        darkPalette.setColor(QPalette::Base, QColor(18, 18, 18));
        darkPalette.setColor(QPalette::AlternateBase, darkColor);
        darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
        darkPalette.setColor(QPalette::ToolTipText, darkColor);
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
        QFile fileSheet(":/Styles/winApp.qss");
        fileSheet.open(QFile::ReadOnly);
        QString styleSheet = QString::fromUtf8(fileSheet.readAll());
        styleSheet = styleSheet.replace("#001133", darkColor.name());
        a.setStyleSheet(styleSheet);
        fileSheet.close();
    }

    // Register QSettings
    QSettings::setDefaultFormat(QSettings::IniFormat);
    MainWindow w(nullptr, isDark);
    w.show();
    return a.exec();
}
