#include "testmainwindow.h"
#include <QtTest/QtTest>

TestMainWindow::TestMainWindow(QObject *parent) : QObject(parent)
{

}


void TestMainWindow::initTestCase()
{
    // w.show();
}


void TestMainWindow::CutName()
{
    QString name("Among Us Momentos Divertidos.mp4");
    int chars = 23;
    QCOMPARE(w.CutName(name, chars), QString("Among Us Momentos Diver....mp4"));
}


void TestMainWindow::cleanupTestCase()
{

}


QTEST_MAIN(TestMainWindow)
