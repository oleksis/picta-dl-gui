#include "configuration.h"
#include "ui_configuration.h"
#include <QDebug>

configuration::configuration(QWidget *parent) : QDialog(parent),
                                                ui(new Ui::configuration)
{
    ui->setupUi(this);

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
    QDir roaming(QStandardPaths::standardLocations(QStandardPaths::AppDataLocation)[0]);
    QFile configFile(roaming.absolutePath().append("\\picta-dl-gui.conf"));

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

        QTextStream in(&configFile);
        QString filePath, cproxy, cport, cproxy_user, cproxy_pass, cpicta_user, cpicta_pass;

        in >> filePath;
        in >> cproxy;
        in >> cport;
        in >> cproxy_user;
        in >> cproxy_pass;
        in >> cpicta_user;
        in >> cpicta_pass;

        filePath.replace('*', ' ');
        defaultDownloadpath = filePath;
        crypto_pass.setKey(crytokey);
        proxy = cproxy.mid(6);
        port = cport.mid(5);
        proxy_user = cproxy_user.mid(7);
        proxy_pass = crypto_pass.decryptToString(cproxy_pass.mid(7));
        picta_user = cpicta_user.mid(7);
        picta_pass = crypto_pass.decryptToString(cpicta_pass.mid(7));

        ui->lnEdit_user_picta->setText(picta_user);
        ui->lnEdit_pass_picta->setText(picta_pass);
        ui->lnEdit_ip_proxy->setText(proxy);
        ui->lnEdit_port_proxy->setText(port);
        ui->lnEdit_user_proxy->setText(proxy_user);
        ui->lnEdit_pass_proxy->setText(proxy_pass);
    }
    else
    {
        QMessageBox::critical(this, qmbTitle, infoText);
        exit(-1);
    }
}

void configuration::saveConfigFile()
{
    QFile configFile(QString(QStandardPaths::standardLocations(QStandardPaths::AppDataLocation)[0]).append("\\picta-dl-gui.conf"));

    if (!configFile.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&configFile);

    QString savedpath, cproxy = ("proxy:"), cport = ("port:"), cproxy_user = ("uproxy:"),
                       cproxy_pass = ("pproxy:"), cpicta_user = ("upicta:"), cpicta_pass = ("ppicta:");

    savedpath = defaultDownloadpath;
    crypto_pass.setKey(crytokey);
    picta_user = ui->lnEdit_user_picta->text();
    picta_pass = ui->lnEdit_pass_picta->text();
    proxy = ui->lnEdit_ip_proxy->text();
    port = ui->lnEdit_port_proxy->text();
    proxy_user = ui->lnEdit_user_proxy->text();
    proxy_pass = ui->lnEdit_pass_proxy->text();
    QString crytopass_picta = crypto_pass.encryptToString(picta_pass), crytopass_proxy;
    if (!proxy_pass.isEmpty())
    {
        crytopass_proxy = crypto_pass.encryptToString(proxy_pass);
    }

    if (!picta_pass.isEmpty())
    {
        crytopass_picta = crypto_pass.encryptToString(picta_pass);
    }

    out << savedpath.append('\n').replace(' ', '*')
        << cproxy.append(proxy + '\n')
        << cport.append(port + '\n')
        << cproxy_user.append(proxy_user + '\n')
        << cproxy_pass.append(crytopass_proxy + '\n')
        << cpicta_user.append(picta_user + '\n')
        << cpicta_pass.append(crytopass_picta + '\n')
        << QString("NOTE: This file must not contain spaces (except this line). "
                   "ALL '*' characters morph into spaces when this file is load.");

    configFile.close();
}

void configuration::on_BntSave_clicked()
{
    saveConfigFile();
    this->close();
}
