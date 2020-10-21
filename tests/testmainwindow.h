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
    void check_crytokey();
    void CutName();
    void withExtension();
    void find_line();
    void cleanupTestCase();

private:
    MainWindow w;
    bool FindCrytoKey(QString);
};

#endif // TESTMAINWINDOW_H
