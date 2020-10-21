#include "testmainwindow.h"
#include <QtTest/QtTest>
#include <QDebug>

TestMainWindow::TestMainWindow(QObject *parent) : QObject(parent)
{
}

void TestMainWindow::initTestCase()
{
    // w.show();
}

bool TestMainWindow::FindCrytoKey(QString filename)
{
    bool finded = false;
    QFile file(filename);
    QTextStream in(&file);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return finded;

    QString header(in.readAll());
    file.close();

    QRegularExpression regex("crytokey = ([0-9]+)");

    finded = regex.match(header).hasMatch();

    return finded;
}

void TestMainWindow::check_crytokey()
{
    QString ROOT(PROJECT_PATH);
    QString mainwindow_path(ROOT + QString("/mainwindow.h"));
    QString configuration_path(ROOT + QString("/configuration.h"));

    QVERIFY(FindCrytoKey(mainwindow_path));
    QVERIFY(FindCrytoKey(configuration_path));
}

void TestMainWindow::withExtension()
{
    QString fileName("Buena Fe - Vales.mp4");

    QCOMPARE(w.withExtension(fileName), QString(".mp4"));

    fileName = QString("No Extension.");

    QCOMPARE(w.withExtension(fileName), QString("."));

    fileName = QString("No Extension");

    QCOMPARE(w.withExtension(fileName), QString(""));
}

void TestMainWindow::CutName()
{
    QString name("Among Us Momentos Divertidos.mp4");
    int chars = 23;

    QCOMPARE(w.CutName(name, chars), QString("Among Us Momentos Diver....mp4"));

    name = QString("Buena Fe - Vales.mp4");

    QCOMPARE(w.CutName(name, chars), QString("Buena Fe - Vales.mp4"));
}

void TestMainWindow::find_line()
{
    // Find line with CaseSensitive
    QString stringline("Buena Fe - Vales.mp4");
    QString stringsearch("Fe");

    QVERIFY(w.find_line(stringline, stringsearch));

    stringsearch = QString("fe");

    QCOMPARE(w.find_line(stringline, stringsearch), false);
}

void TestMainWindow::cleanupTestCase()
{
}

QTEST_MAIN(TestMainWindow)
