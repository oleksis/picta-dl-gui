#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QDialog>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSettings>
#include <QStandardPaths>
#include <QMessageBox>
#include <QTextStream>
#include "simplecrypt.h"

namespace Ui
{
    class configuration;
}

class configuration : public QDialog
{
    Q_OBJECT

public:
    explicit configuration(QWidget *parent = nullptr, const QString &filename = QString("picta-dl-gui.conf"));
    ~configuration();

private slots:
    void on_BntCancel_clicked();
    void on_BntSave_clicked();

private:
    Ui::configuration *ui;

    QString proxy;
    QString port;
    QString proxy_user;
    QString proxy_pass;
    QString picta_user;
    QString picta_pass;
    SimpleCrypt crypto_pass;
    quint64 crytokey = 1596880491598749;
    QString defaultDownloadpath;
    QString pictaGuiConfigFile;
    bool systray;
    bool notification;
    bool envpictadl;
    bool envffmpeg;

    void loadConfigFile();
    void saveConfigFile();
};

#endif // CONFIGURATION_H
