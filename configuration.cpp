#include "configuration.h"
#include "ui_configuration.h"
#include <QDebug>

configuration::configuration(QWidget *parent, const QString &filename) :
    QDialog(parent),
    ui(new Ui::configuration),
    pictaGuiConfigFile(filename)
{
    ui->setupUi(this);
    roamingDirPath = QDir(QStandardPaths::standardLocations(QStandardPaths::AppDataLocation)[0]);
    loadConfigFile();
}

configuration::~configuration()
{
    delete ui;
}

void configuration::on_BntCancel_clicked()
{
    reject();
}

void configuration::loadConfigFile()
{
    QFile configFile(roamingDirPath.absolutePath().append("/") + pictaGuiConfigFile);
    QFileInfo configInfo(configFile);
    QSettings settings(configInfo.absoluteFilePath(), QSettings::IniFormat);
    QString qmbTitle("Error fatal");
    QString infoText("No se puede leer el archivo de configuración picta-dl-gui.conf\n"
                     "Aplicación terminada.\n\n"
                     "Vuelva a ejecutar el programa. Si vuelve a recibir el mismo "
                     "error, ejecútelo como administrador una vez. Es muy probable que "
                     "eso resuelva el problema.");

    if (configFile.exists())
    {
        if (!configFile.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QMessageBox::critical(this, qmbTitle, infoText);
            exit(-1);
        }

        configFile.close();
        QString filePath, cproxy, cport, cproxy_user, cproxy_pass, cpicta_user, cpicta_pass;
        settings.beginGroup("General");
        filePath = settings.value("savedPath").toString();
        cproxy = settings.value("proxy").toString();
        cport = settings.value("port").toString();
        cproxy_user = settings.value("uproxy").toString();
        cproxy_pass = settings.value("pproxy").toString();
        cpicta_user = settings.value("upicta").toString();
        cpicta_pass = settings.value("ppicta").toString();
        systray = settings.value("systray").toBool();
        notification = settings.value("notification").toBool();
        envpictadl = settings.value("envpictadl").toBool();
        envffmpeg = settings.value("envffmpeg").toBool();
        settings.endGroup();
        defaultDownloadpath = filePath;
        crypto_pass.setKey(crytokey);
        proxy = cproxy;
        port = cport;
        proxy_user = cproxy_user;
        proxy_pass = crypto_pass.decryptToString(cproxy_pass);
        picta_user = cpicta_user;
        picta_pass = crypto_pass.decryptToString(cpicta_pass);
        ui->lnEdit_user_picta->setText(picta_user);
        ui->lnEdit_pass_picta->setText(picta_pass);
        ui->lnEdit_ip_proxy->setText(proxy);
        ui->lnEdit_port_proxy->setText(port);
        ui->lnEdit_user_proxy->setText(proxy_user);
        ui->lnEdit_pass_proxy->setText(proxy_pass);

        if (systray)
            ui->checkSystray->setChecked(true);

        if (notification)
            ui->checkNotified->setChecked(true);

        if (envpictadl)
            ui->checkEnvPictadl->setChecked(true);

        if (envffmpeg)
            ui->checkEnvFFmpeg->setChecked(true);
    }
    else
    {
        QMessageBox::critical(this, qmbTitle, infoText);
        exit(-1);
    }
}

void configuration::saveConfigFile()
{
    QFile configFile(roamingDirPath.absolutePath().append("/") + pictaGuiConfigFile);
    QFileInfo configInfo(configFile);
    QSettings settings(configInfo.absoluteFilePath(), QSettings::IniFormat);

    if (!settings.isWritable())
        return;

    QString savedpath, cproxy, cport, cproxy_user, cproxy_pass, cpicta_user, cpicta_pass;
    savedpath = defaultDownloadpath;
    crypto_pass.setKey(crytokey);
    cpicta_user = ui->lnEdit_user_picta->text();
    cpicta_pass = ui->lnEdit_pass_picta->text();
    cproxy = ui->lnEdit_ip_proxy->text();
    cport = ui->lnEdit_port_proxy->text();
    cproxy_user = ui->lnEdit_user_proxy->text();
    cproxy_pass = ui->lnEdit_pass_proxy->text();
    systray = ui->checkSystray->isChecked() ? true : false;
    notification = ui->checkNotified->isChecked() ? true : false;
    envpictadl = ui->checkEnvPictadl->isChecked() ? true : false;
    envffmpeg = ui->checkEnvFFmpeg->isChecked() ? true : false;
    QString crytopass_picta = crypto_pass.encryptToString(cpicta_pass), crytopass_proxy;

    if (!cproxy_pass.isEmpty())
        crytopass_proxy = crypto_pass.encryptToString(cproxy_pass);

    if (!cpicta_pass.isEmpty())
        crytopass_picta = crypto_pass.encryptToString(cpicta_pass);

    settings.beginGroup("General");
    settings.setValue("savedPath", savedpath);
    settings.setValue("proxy", cproxy);
    settings.setValue("port", cport);
    settings.setValue("uproxy", cproxy_user);
    settings.setValue("pproxy", crytopass_proxy);
    settings.setValue("upicta", cpicta_user);
    settings.setValue("ppicta", crytopass_picta);
    settings.setValue("systray", systray);
    settings.setValue("notification", notification);
    settings.setValue("envpictadl", envpictadl);
    settings.setValue("envffmpeg", envffmpeg);
    settings.endGroup();
}

void configuration::on_BntSave_clicked()
{
    saveConfigFile();
    this->close();
}
