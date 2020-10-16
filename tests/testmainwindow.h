#ifndef TESTMAINWINDOW_H
#define TESTMAINWINDOW_H

#include "mainwindow.h"
#include <QObject>

class TestMainWindow : public QObject
{
    Q_OBJECT
public:
    explicit TestMainWindow(QObject *parent = nullptr);

private slots:
    void initTestCase();
    void CutName();
    void cleanupTestCase();

private:
    MainWindow w;


};

#endif // TESTMAINWINDOW_H
