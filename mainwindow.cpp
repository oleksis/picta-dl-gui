#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) : 
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    pictaGuiConfigFile = QString("picta-dl-gui.conf");
    pictaConfigFile = QString("picta-dl.conf");
    configure();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_cmmd_config_clicked()
{
    configuration conf;
    conf.setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    conf.exec();
}

void MainWindow::configure()
{
    QDir roaming(QStandardPaths::standardLocations(QStandardPaths::AppDataLocation)[0]);
    QString defaultpath(QStandardPaths::standardLocations(QStandardPaths::DownloadLocation)[0]);
    QChar backslash(92);

    if (!roaming.exists())
    {
        roaming.mkpath(roaming.absolutePath());
        ui->cmmd_help->click();
    }

    pictadlDLLpath = roaming.absolutePath().append("\\picta-dl.exe");
    ffmpegDLLpath = roaming.absolutePath().append("\\ffmpeg.exe");
    defaultDownloadpath = defaultpath;

    ui->lineEdit_Location->setText(defaultpath.replace("/", backslash));
    crypto_pass.setKey(crytokey);

    if (!loadConfigFile(roaming))
        createConfigFile(roaming);

    checkExistenceOfMainProcess();

    //SystrayIcon
    createActions();
    createTrayIcon();
    trayIcon->setIcon(QIcon(QString::fromUtf8(":/Logos/picta_dl_gui_icon.ico")));
    trayIcon->show();
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

    ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->tableWidget->verticalHeader()->setDefaultAlignment(Qt::AlignVCenter);
}

bool MainWindow::loadConfigFile(QDir &roaming)
{
    QFile configFile(roaming.absolutePath().append("/") + pictaGuiConfigFile);
    QFileInfo configInfo(configFile);
    QSettings settings(configInfo.absoluteFilePath(), QSettings::IniFormat);

    if (configFile.exists())
    {
        if (!configFile.open(QIODevice::ReadOnly | QIODevice::Text))
            return false;

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

        QChar backslash(92);
        ui->lineEdit_Location->setText(filePath.replace("/", backslash));

        crypto_pass.setKey(crytokey);
        proxy = cproxy;
        port = cport;
        proxy_user = cproxy_user;
        proxy_pass = crypto_pass.decryptToString(cproxy_pass);
        picta_user = cpicta_user;
        picta_pass = crypto_pass.decryptToString(cpicta_pass);

        return true;
    }

    return false;
}

void MainWindow::createConfigFile(QDir &roaming)
{
    QFile configFile(roaming.absolutePath().append("/") + pictaGuiConfigFile);
    QFileInfo configInfo(configFile);
    QSettings settings(configInfo.absoluteFilePath(), QSettings::IniFormat);

    if (!settings.isWritable())
        return;

    QString savedpath, cproxy, cport, cproxy_user,
        cproxy_pass, cpicta_user, cpicta_pass;
    savedpath = defaultDownloadpath;

    settings.beginGroup("General");

    settings.setValue("savedPath", savedpath);
    settings.setValue("proxy", cproxy);
    settings.setValue("port", cport);
    settings.setValue("uproxy", cproxy_user);
    settings.setValue("pproxy", cproxy_pass);
    settings.setValue("upicta", cpicta_user);
    settings.setValue("ppicta", cpicta_pass);
    settings.setValue("systray", false);
    settings.setValue("notification", false);
    settings.setValue("envpictadl", false);
    settings.setValue("envffmpeg", false);

    settings.endGroup();

    //Creating default picta.conf

    QFile configPictaFile(roaming.absolutePath().append("/") + pictaConfigFile);

    if (!configPictaFile.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out_picta_conf(&configPictaFile);

    QString PictaFormat = ("-o \"%(title)s.%(ext)s\"");
    QString ffmpeg_conf = (ffmpegDLLpath);
    // If Win32
    QChar backslash(92);
    ffmpeg_conf = ffmpeg_conf.replace("/", backslash);
    ffmpeg_conf.prepend("\"");
    ffmpeg_conf.append("\"");

    QString FFmpegPath = ("--ffmpeg-location " + ffmpeg_conf);
    out_picta_conf << "# Lines starting with # are comments\n\n"
                   << "# Format video output <video>.<extension>\n"
                   << PictaFormat.append("\n\n")
                   << "# FFmpeg PATH\n"
                   << FFmpegPath;

    configPictaFile.close();
}

void MainWindow::checkExistenceOfMainProcess()
{
    QFile upxFile(pictadlDLLpath);
    QFile upxFile2(ffmpegDLLpath);

    QString qmbTitle("Error fatal");
    QString infoText("Aplicación terminada.\n\n"
                     "Vuelva a ejecutar el programa. Si vuelve a recibir el mismo "
                     "error, ejecútelo como administrador una vez. Es muy probable que "
                     "eso resuelva el problema.");

    if (!upxFile.exists())
    {
        QFile rcFileUPX(":/picta-dl.dll");
        rcFileUPX.copy(upxFile.fileName());
        rcFileUPX.rename(upxFile.fileName(), upxFile.fileName().replace("dll", "exe"));
        if (!upxFile.exists())
        {
            QMessageBox::critical(this, qmbTitle, QString("No se puede crear el proceso runtime picta-dl.dll\n") + infoText);
            exit(-1);
        }
    }

    if (!upxFile2.exists())
    {
        QFile rcFileUPX2("://ffmpeg.dll");
        rcFileUPX2.copy(upxFile2.fileName());
        rcFileUPX2.rename(upxFile2.fileName(), upxFile2.fileName().replace("dll", "exe"));
        if (!upxFile2.exists())
        {
            QMessageBox::critical(this, qmbTitle, QString("No se puede crear el proceso runtime ffmpeg.dll\n") + infoText);
            exit(-1);
        }
    }
}

void MainWindow::saveConfigFile()
{
    QFile configFile(QString(QStandardPaths::standardLocations(QStandardPaths::AppDataLocation)[0]).append("/") + pictaGuiConfigFile);
    QFileInfo configInfo(configFile);
    QSettings settings(configInfo.absoluteFilePath(), QSettings::IniFormat);

    if (!settings.isWritable())
        return;

    QString savedpath;
    savedpath = defaultDownloadpath;
    crypto_pass.setKey(crytokey);

    QString crytopass_picta = crypto_pass.encryptToString(picta_pass), crytopass_proxy;

    if (!proxy_pass.isEmpty())
    {
        crytopass_proxy = crypto_pass.encryptToString(proxy_pass);
    }

    if (!picta_pass.isEmpty())
    {
        crytopass_picta = crypto_pass.encryptToString(picta_pass);
    }

    settings.beginGroup("General");

    settings.setValue("savedPath", savedpath);
    settings.setValue("proxy", proxy);
    settings.setValue("port", port);
    settings.setValue("uproxy", proxy_user);
    settings.setValue("pproxy", crytopass_proxy);
    settings.setValue("upicta", picta_user);
    settings.setValue("ppicta", crytopass_picta);
    settings.setValue("systray", systray);
    settings.setValue("notification", notification);
    settings.setValue("envpictadl", envpictadl);
    settings.setValue("envffmpeg", envffmpeg);

    settings.endGroup();
}

void MainWindow::on_toolButton_clicked()
{
    QString downloadFolder(QFileDialog::getExistingDirectory(this,
                                                             "Ubicación de Descarga",
                                                             defaultDownloadpath,
                                                             QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks));
    if (downloadFolder.isEmpty())
        return;

    QChar backslash(92);
    QDir roaming(QStandardPaths::standardLocations(QStandardPaths::AppDataLocation)[0]);
    loadConfigFile(roaming);
    defaultDownloadpath = downloadFolder;
    ui->lineEdit_Location->setText(downloadFolder.replace("/", backslash));
    saveConfigFile();
}

bool MainWindow::PasteFromClipboard()
{
    const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();
    QString CopyTextClip = clipboard->text();

    QString qmbTitle("Error al Pegar");
    QString infoText("Sólo puede pegar Urls que sean del sitio de Picta ejemplo:\n\n"
                     "https://www.picta.cu/...");

    if (mimeData->hasImage())
    {
        QMessageBox::information(this, qmbTitle, QString("No se puede pegar una imagen\n\n") + infoText);
        QApplication::alert(this);
        return false;
    }
    else if (mimeData->hasHtml())
    {
        pasteUrl = CopyTextClip;
        return true;
    }
    else if (mimeData->hasUrls())
    {
        QMessageBox::information(this, qmbTitle, QString("Solo puede pegar datos de tipo URLs\n\n") + infoText);
        QApplication::alert(this);
        return false;
    }
    else if (mimeData->hasText())
    {
        pasteUrl = CopyTextClip;
        return true;
    }
    else
    {
        return false;
    }
}

void MainWindow::on_bnt_clipboard_clicked()
{
    QString qmbTitle("Error al Pegar");
    QString infoText("Sólo puede pegar Urls que sean del sitio de Picta ejemplo:\n\n"
                     "https://www.picta.cu/...");

    if (PasteFromClipboard())
    {
        QRegularExpression regex("((?:https?|http)://\\S+)");
        if (regex.match(pasteUrl).hasMatch())
        {
            QString pictaurl("picta.cu/medias");
            if (find_line(pasteUrl, pictaurl))
            {
                ui->lineEdit_url->setText(pasteUrl);
            }
            else
            {
                QMessageBox::information(this, qmbTitle, QString("No es una URL válida de Picta\n\n") + infoText);
                QApplication::alert(this);
            }
        }
        else
        {
            QMessageBox::information(this, qmbTitle, QString("No es una URL válida\n\n") + infoText);
            QApplication::alert(this);
        }
    }
}

bool MainWindow::find_line(QString stringline, QString stringsearch)
{
    int search = stringline.indexOf(stringsearch, 0, Qt::CaseSensitive);

    if (search != -1)
    {
        return true;
    }

    return false;
}

void MainWindow::get_filename()
{
    QString qmbTitle("Error al Pegar");
    QString infoText("Sólo puede pegar Urls que sean del sitio de Picta ejemplo:\n\n"
                     "https://www.picta.cu/...");

    if (!ui->lineEdit_url->text().isEmpty())
    {
        QDir roaming(QStandardPaths::standardLocations(QStandardPaths::AppDataLocation)[0]);
        QString picta_conf = roaming.absolutePath().append("/picta-dl.conf");
        QString playlist = roaming.absolutePath().append("/playlist.txt");
        ui->tableWidget->setRowCount(0);
        process_count = 0;
        //Animated Process list
        progressAni.setFileName(":/Logos/refresh_rotate.gif");
        progressAni.start();
        ui->lbl_process->setText("<html><head/><body><p><span style=\" font-weight:600;\">Procesando URL...</span></p></body></html>");
        connect(&progressAni, SIGNAL(frameChanged(int)), this, SLOT(setRefreshIcon()));
        //Prepare process for url list
        pictadl.setProcessChannelMode(QProcess::MergedChannels);
        QStringList args;

        if (envpictadl)
        {
            pictadl.setProgram("picta-dl.exe");
        }
        else
        {
            pictadl.setProgram(pictadlDLLpath);
        }

        if (envffmpeg)
        {
            args << "--ignore-config"
                 << "-o"
                 << "%(title)s.%(ext)s";
        }
        else
        {
            args << "--config-location" << picta_conf;
        }

        args << "--abort-on-error"
             << "--socket-timeout"
             << "10"
             << "--retries"
             << "6"
             << "--get-filename";

        if (!ui->chckBox_Playlist->isChecked())
        {
            args << "--no-playlist";
        }
        else
        {
            args << "--yes-playlist";
        }

        if (ui->chckBox_Onlyaudio->isChecked())
        {
            args << "-x"
                 << "--audio-quality"
                 << "0";
        }

        if (!proxy.isEmpty())
        {
            if (!proxy_user.isEmpty() && !proxy_pass.isEmpty())
            {
                args << "--proxy"
                     << "https://" + proxy_user + ":" + proxy_pass + "@" + proxy + ":" + port;
            }
            else
            {
                args << "--proxy"
                     << "https://" + proxy + ":" + port;
            }
        }

        args << "-u" << picta_user << "-p" << picta_pass << pasteUrl;

        pictadl.setArguments(args);
        pictadl.setStandardOutputFile(playlist, QIODevice::Truncate);
        pictadl.start();
        stopped = false;
        ui->cmmd_process->setEnabled(false);
        ui->cmmd_download->setEnabled(false);
        ui->cmmd_stop->setEnabled(true);
        ui->cmmd_dlte->setEnabled(false);

        connect(&pictadl, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(onFinishGetfilenames()));
    }
    else
    {
        QMessageBox::information(this, qmbTitle, QString("Debe pegar una URL válida de Picta\n\n") + infoText);
        QApplication::alert(this);
    }
}

void MainWindow::onFinishGetfilenames()
{
    pictadl.kill();
    pictadl.terminate();
    pictadl.close();
    process_count++;

    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableWidget->verticalHeader()->setDefaultAlignment(Qt::AlignVCenter);

    if (process_count < 2)
    {
        //Read processed list
        QDir roaming(QStandardPaths::standardLocations(QStandardPaths::AppDataLocation)[0]);
        QFile playlistfile(roaming.absolutePath().append("/playlist.txt"));
        playlistfile.open(QIODevice::ReadWrite | QIODevice::Text);
        QTextStream listfile(&playlistfile);
        QString ListItem, ShortName;

        do
        {
            ListItem = listfile.readLine();
            if (!ListItem.contains("ERROR:", Qt::CaseSensitive) && !ListItem.isEmpty())
            {
                ShortName = CutName(ListItem, 23);
                ui->tableWidget->insertRow(ui->tableWidget->rowCount());

                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, ColFile, new QTableWidgetItem(ShortName));
                ui->tableWidget->item(ui->tableWidget->rowCount() - 1, ColFile)->setToolTip(ListItem);
                ui->tableWidget->item(ui->tableWidget->rowCount() - 1, ColFile)->setTextAlignment(Qt::AlignLeft);
                ui->tableWidget->item(ui->tableWidget->rowCount() - 1, ColFile)->setTextAlignment(Qt::AlignVCenter);

                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, ColVideo, new QTableWidgetItem("➖"));
                ui->tableWidget->item(ui->tableWidget->rowCount() - 1, ColVideo)->setTextAlignment(Qt::AlignCenter);

                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, ColAudio, new QTableWidgetItem("➖"));
                ui->tableWidget->item(ui->tableWidget->rowCount() - 1, ColAudio)->setTextAlignment(Qt::AlignCenter);

                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, Colsubtitle, new QTableWidgetItem("➖"));
                ui->tableWidget->item(ui->tableWidget->rowCount() - 1, Colsubtitle)->setTextAlignment(Qt::AlignCenter);

                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, ColVelocidad, new QTableWidgetItem("➖"));
                ui->tableWidget->item(ui->tableWidget->rowCount() - 1, ColVelocidad)->setTextAlignment(Qt::AlignCenter);

                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, ColProcess, new QTableWidgetItem("➖"));
                ui->tableWidget->item(ui->tableWidget->rowCount() - 1, ColProcess)->setTextAlignment(Qt::AlignCenter);
            }

        } while (!listfile.atEnd());

        playlistfile.close();
        progressAni.stop();
        ui->cmmd_process->setIcon(QIcon(":/Logos/refreshbutton.png"));
        ui->lbl_process->setText("<html><head/><body><p><span style=\" font-weight:600;\">Procesar URL</span></p></body></html>");

        if (!ListItem.contains("ERROR:", Qt::CaseSensitive) && !ListItem.contains("Failed to execute script", Qt::CaseInsensitive))
        {
            if (!stopped)
            {
                QString qmlmessage = "Se ha terminado de procesar la lista de descarga";
                if (notification)
                {
                    showNotifiedMessage(qmlmessage);
                }
                else
                {
                    this->showNormal();
                    QApplication::alert(this);
                    QMessageBox::information(this, "Procesado terminado", qmlmessage);
                }
                ui->cmmd_stop->setEnabled(false);
                ui->cmmd_download->setEnabled(true);
                ui->cmmd_dlte->setEnabled(true);
                ui->cmmd_process->setEnabled(true);
            }
        }
        else
        {
            URL_Process_Error(ListItem);
        }
    }
}

void MainWindow::URL_Process_Error(QString error)
{
    QString qmbTitle("Error al Procesar la URL:\n");
    QString qmbError;

    if (error.contains("ERROR: This playlist is only available for registered users", Qt::CaseSensitive))
    {
        qmbError = "Las listas de reproducción es sólo para usuarios registrados\n"
                   "Revise su usuario y contraseña.";

        ShowErrorMessage(qmbTitle, qmbError);
    }
    else if (error.contains("ERROR: Playlist no exists!", Qt::CaseSensitive))
    {
        qmbError = "No existe la lista de reproducción que intenta procesar\n"
                   "Revise la lista de reproducción en el sitio";

        ShowErrorMessage(qmbTitle, qmbError);
    }
    else if (error.contains("ERROR: no suitable InfoExtractor for URL", Qt::CaseSensitive))
    {
        qmbError = "Es incorrecta la URL que intenta procesar\n"
                   "Revise que este correcta en el sitio de Picta";

        ShowErrorMessage(qmbTitle, qmbError);
    }
    else if (error.contains("ERROR: Unable to download JSON metadata: <urlopen error [SSL: CERTIFICATE_VERIFY_FAILED]", Qt::CaseSensitive))
    {
        qmbError = "Ha habido un error de certificado SSL de su sistema\n"
                   "es posible que tenga que actualizar su Sistema";

        ShowErrorMessage(qmbTitle, qmbError);
    }
    else if (error.contains("ERROR: Unable to download JSON metadata: <urlopen error Tunnel connection failed: 407 Proxy Authentication Required", Qt::CaseSensitive))
    {
        qmbError = "Ha habido un error de autenticación con el servidor Proxy\n"
                   "Revise usuario y contraseña del Proxy";

        ShowErrorMessage(qmbTitle, qmbError);
    }
    else if (error.contains("ERROR: Unable to download JSON metadata:", Qt::CaseSensitive))
    {
        qmbError = "Ha habido un error de conexión con el servidor\n"
                   "Revise su conexión a la RED y a Picta";

        ShowErrorMessage(qmbTitle, qmbError);
    }
    else if (error.contains("ERROR: Failed to download MPD manifest:", Qt::CaseSensitive))
    {
        qmbError = "Ha habido un error de conexión con el servidor\n"
                   "Revise su conexión a la RED y a Picta";

        ShowErrorMessage(qmbTitle, qmbError);
    }
    else if (error.contains("ERROR: Cannot find video!", Qt::CaseSensitive) || error.isEmpty())
    {
        qmbError = "¡No se ha encontrado el vídeo en el sitio!\n"
                   "Revise que este correcta la URL en el sitio";

        ShowErrorMessage(qmbTitle, qmbError);
    }
    else
    {
        qmbError = "¡Ha ocurrido un error inesperado!\n"
                   "Vuelva a intentarlo dentro de un rato";

        ShowErrorMessage(qmbTitle, qmbError);
    }

    ui->cmmd_process->setEnabled(true);
    ui->cmmd_stop->setEnabled(false);
    ui->cmmd_dlte->setEnabled(true);
}

void MainWindow::on_cmmd_process_clicked()
{
    QString qmbTitle("Error al Procesar la URL");

    QDir roaming(QStandardPaths::standardLocations(QStandardPaths::AppDataLocation)[0]);
    loadConfigFile(roaming);

    if (!IsNetworkConnected())
    {
        QMessageBox::critical(this, qmbTitle, "¡No está conectado a la Red!\n\n"
                                              "Revise su conexión de RED/HUB/Router/WIFI o Datos!");
        QApplication::alert(this);
    }
    else if (picta_pass.isEmpty() || picta_user.isEmpty())
    {

        QMessageBox::warning(this, qmbTitle, "¡Debe configurar su usuario y contraseña de Picta!\n\n"
                                             "Revise que tenga configurado el usuario y contraseña del sitio de Picta.");
        QApplication::alert(this);
    }
    else if (envpictadl && !ExistsProgram("picta-dl"))
    {
        QMessageBox::warning(this, qmbTitle, "¡No se ha encontrado picta-dl en su Sistema!\n\n"
                                             "¡Revise que este bien configurado en las variables de entorno\n"
                                             "y picta-dl esté disponible en su ubicación!");
        QApplication::alert(this);
    }
    else
    {
        get_filename();
    }
}

void MainWindow::setRefreshIcon()
{
    ui->cmmd_process->setIcon(QIcon(progressAni.currentPixmap()));
}

void MainWindow::Downloadfiles()
{
    QDir roaming(QStandardPaths::standardLocations(QStandardPaths::AppDataLocation)[0]);
    QString picta_conf = roaming.absolutePath().append("/picta-dl.conf");
    download_count = 0;
    stopped = false;
    ui->cmmd_process->setEnabled(false);
    //Animated Process list
    downloadsAni.setFileName(":/Logos/download_animated.gif");
    downloadsAni.start();
    ui->lbl_download->setText("<html><head/><body><p><span style=\" font-weight:600;\">Descargando...</span></p></body></html>");
    connect(&downloadsAni, SIGNAL(frameChanged(int)), this, SLOT(setDownloadIcon()));

    //Prepare arguments
    QStringList args;
    if (envpictadl)
    {
        pictadlfiles.setProgram("picta-dl.exe");
    }
    else
    {
        pictadlfiles.setProgram(pictadlDLLpath);
    }

    if (envffmpeg)
    {
        args << "--ignore-config"
             << "-o"
             << "%(title)s.%(ext)s";
    }
    else
    {
        args << "--config-location" << picta_conf;
    }
    args << "--abort-on-error"
         << "--socket-timeout"
         << "10"
         << "--retries"
         << "6";

    if (!ui->chckBox_Playlist->isChecked())
    {
        args << "--no-playlist";
    }
    else
    {
        args << "--yes-playlist";
    }

    if (ui->chckBox_Subt->isChecked())
    {
        args << "--write-sub";
    }

    if (ui->chckBox_Onlyaudio->isChecked())
    {
        args << "-x"
             << "--audio-quality"
             << "0";
    }

    if (!ui->chckBox_Onlyaudio->isChecked())
    {
        if (ui->radBnt_Alta->isChecked())
        {
            args << "-f"
                 << "bestvideo+bestaudio";
        }
        else if (ui->radBnt_Media->isChecked())
        {
            args << "-f"
                 << "2+5/1+bestaudio/best";
        }
        else
        {
            args << "-f"
                 << "4+5/2+bestaudio/best";
        }
    }

    if (!proxy.isEmpty())
    {
        if (!proxy_user.isEmpty() && !proxy_pass.isEmpty())
        {
            args << "--proxy"
                 << "https://" + proxy_user + ":" + proxy_pass + "@" + proxy + ":" + port;
        }
        else
        {
            args << "--proxy"
                 << "https://" + proxy + ":" + port;
        }
    }

    args << "-u" << picta_user << "-p" << picta_pass << pasteUrl;

    QString filePath = ui->lineEdit_Location->text();
    QChar backslash(92);
    args << "-o" << filePath.append(backslash) + "%(title)s.%(ext)s";

    IsVideo = false;
    IsAudio = false;
    errString = "";
    //Prepare Process for download
    pictadlfiles.setArguments(args);
    pictadlfiles.setProcessChannelMode(QProcess::MergedChannels);
    pictadlfiles.start();
    ui->cmmd_download->setEnabled(false);
    ui->cmmd_stop->setEnabled(true);
    ui->cmmd_dlte->setEnabled(false);

    connect(&pictadlfiles, &QProcess::readyReadStandardOutput, this, [&]() {
        stdoutString = pictadlfiles.readAllStandardOutput();
        //qDebug() << "Stdout: " << stdoutString;

        if (stdoutString.contains("WARNING: You have requested multiple formats but ffmpeg or avconv are not installed", Qt::CaseInsensitive))
        {
            errString = stdoutString;
            emit onFinishDowloadfiles();
            ui->cmmd_stop->setEnabled(false);
            ui->cmmd_process->setEnabled(true);
            ui->cmmd_download->setEnabled(false);
            ui->cmmd_dlte->setEnabled(true);
        }

        if (stdoutString.contains("Writing video subtitles", Qt::CaseSensitive))
        {
            ui->tableWidget->item(itemlist, Colsubtitle)->setText("✔");
            ui->tableWidget->item(itemlist, Colsubtitle)->setTextColor(QColor(0, 170, 0));
            ui->tableWidget->item(itemlist, Colsubtitle)->setTextAlignment(Qt::AlignCenter);
        }

        if (stdoutString.contains("WARNING: video doesn't have subtitles", Qt::CaseSensitive))
        {
            ui->tableWidget->item(itemlist, Colsubtitle)->setText("❌");
            ui->tableWidget->item(itemlist, Colsubtitle)->setTextColor(QColor(170, 0, 0));
            ui->tableWidget->item(itemlist, Colsubtitle)->setTextAlignment(Qt::AlignCenter);
        }

        //if file has already been downloaded and merged jump to the next row
        if (stdoutString.contains("has already been downloaded and merged", Qt::CaseSensitive) && (stdoutString.contains(".mp4") || stdoutString.contains("f4.webm")))
        {
            IsVideo = false;
            IsAudio = false;
            ui->tableWidget->item(itemlist, ColVideo)->setText("100%");
            ui->tableWidget->item(itemlist, ColVideo)->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget->item(itemlist, ColAudio)->setText("100%");
            ui->tableWidget->item(itemlist, ColAudio)->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget->item(itemlist, ColProcess)->setText("✔");
            ui->tableWidget->item(itemlist, ColProcess)->setTextColor(QColor(0, 170, 0));
            ui->tableWidget->item(itemlist, ColProcess)->setTextAlignment(Qt::AlignCenter);
            itemlist++;
        }

        //if the video has already been downloaded jump to corresponding audio column
        else if (stdoutString.contains("has already been downloaded", Qt::CaseSensitive) && (stdoutString.contains(".mp4") || stdoutString.contains("f4.webm")))
        {
            IsVideo = false;
            ui->tableWidget->item(itemlist, ColVideo)->setText("100%");
            ui->tableWidget->item(itemlist, ColVideo)->setTextAlignment(Qt::AlignCenter);
            IsAudio = true;
        }

        if (stdoutString.contains("[download] Destination:", Qt::CaseSensitive) && (stdoutString.contains(".mp4") || stdoutString.contains("f4.webm")))
        {
            IsVideo = true;
            IsAudio = false;
        }
        else if (stdoutString.contains("[download] Destination:", Qt::CaseSensitive) && (stdoutString.contains(".m4a") || stdoutString.contains("f5.webm")))
        {
            IsVideo = false;
            IsAudio = true;
        }

        if (stdoutString.contains("[download] 100%", Qt::CaseSensitive) && IsVideo && !IsAudio)
        {
            IsVideo = false;
        }

        if (stdoutString.contains("[download] 100%", Qt::CaseSensitive) && IsAudio && !IsVideo)
        {
            IsAudio = true;
            IsVideo = false;
        }

        if (stdoutString.contains("at", Qt::CaseSensitive) && stdoutString.contains("ETA", Qt::CaseSensitive))
        {
            ItemOut.clear();
            ItemOut = stdoutString.split(QRegExp("\\s+"), QString::SkipEmptyParts);

            ui->tableWidget->item(itemlist, ColVelocidad)->setText(ItemOut.at(5));
            ui->tableWidget->item(itemlist, ColVelocidad)->setTextAlignment(Qt::AlignCenter);

            if (IsVideo)
            {
                ui->tableWidget->item(itemlist, ColVideo)->setText(ItemOut.at(1) + " de (" + ItemOut.at(3) + ")");
                ui->tableWidget->item(itemlist, ColVideo)->setTextAlignment(Qt::AlignCenter);
            }
            if (IsAudio)
            {
                ui->tableWidget->item(itemlist, ColAudio)->setText(ItemOut.at(1) + " de (" + ItemOut.at(3) + ")");
                ui->tableWidget->item(itemlist, ColAudio)->setTextAlignment(Qt::AlignCenter);
            }
        }

        //Avoid don't write 100% on finished download
        if (stdoutString.contains("100% of", Qt::CaseSensitive) && stdoutString.contains("in", Qt::CaseSensitive))
        {
            ItemOut.clear();
            ItemOut = stdoutString.split(QRegExp("\\s+"), QString::SkipEmptyParts);

            if (IsVideo)
            {
                ui->tableWidget->item(itemlist, ColVideo)->setText(ItemOut.at(1) + " de (" + ItemOut.at(3) + ")");
                ui->tableWidget->item(itemlist, ColVideo)->setTextAlignment(Qt::AlignCenter);
            }
            if (IsAudio)
            {
                ui->tableWidget->item(itemlist, ColAudio)->setText(ItemOut.at(1) + " de (" + ItemOut.at(3) + ")");
                ui->tableWidget->item(itemlist, ColAudio)->setTextAlignment(Qt::AlignCenter);
            }
        }

        if (stdoutString.contains("[ffmpeg] Merging formats", Qt::CaseSensitive))
        {
            ui->tableWidget->item(itemlist, ColProcess)->setText("Procesando...");
            ui->tableWidget->item(itemlist, ColProcess)->setTextColor(QColor(170, 0, 0));
            ui->tableWidget->item(itemlist, ColProcess)->setTextAlignment(Qt::AlignCenter);
        }

        if (stdoutString.contains("Deleting original file", Qt::CaseSensitive) && (stdoutString.contains("m4a", Qt::CaseSensitive) || stdoutString.contains("f5.webm")))
        {
            ui->tableWidget->item(itemlist, ColProcess)->setText("✔");
            ui->tableWidget->item(itemlist, ColProcess)->setTextColor(QColor(0, 170, 0));
            ui->tableWidget->item(itemlist, ColProcess)->setTextAlignment(Qt::AlignCenter);
            itemlist++;
        }

        if (stdoutString.contains("[ffmpeg] Post-process file", Qt::CaseSensitive) && stdoutString.contains("m4a", Qt::CaseSensitive))
        {
            ui->tableWidget->item(itemlist, ColProcess)->setText("❌");
            ui->tableWidget->item(itemlist, ColProcess)->setTextColor(QColor(170, 0, 0));
            ui->tableWidget->item(itemlist, ColProcess)->setTextAlignment(Qt::AlignCenter);
            itemlist++;
        }

        if (stdoutString.contains("ERROR:", Qt::CaseSensitive))
        {
            errString = stdoutString;
        }
    });

    connect(&pictadlfiles, SIGNAL(finished(int)), this, SLOT(onFinishDowloadfiles()));
}

void MainWindow::setDownloadIcon()
{
    ui->cmmd_download->setIcon(QIcon(downloadsAni.currentPixmap()));
}

void MainWindow::on_cmmd_download_clicked()
{
    QString qmbTitle("Error al Descargar archivos");

    QDir roaming(QStandardPaths::standardLocations(QStandardPaths::AppDataLocation)[0]);
    loadConfigFile(roaming);

    if (!IsNetworkConnected())
    {
        QMessageBox::critical(this, qmbTitle, "¡No está conectado a la Red!\n\n"
                                              "Revise su conexión de RED/HUB/Router/WIFI o Datos!");
        QApplication::alert(this);
    }
    else if (picta_pass.isEmpty() || picta_user.isEmpty())
    {

        QMessageBox::warning(this, qmbTitle, "¡Debe configurar su usuario y contraseña de Picta!\n\n"
                                             "Revise que tenga configurado el usuario y contraseña del sitio de Picta.");
        QApplication::alert(this);
    }
    else if (envffmpeg && !ExistsProgram("ffmpeg"))
    {
        QMessageBox::warning(this, qmbTitle, "¡No se ha encontrado ffmpeg en su Sistema!\n\n"
                                             "¡Revise que este bien configurado en las variables de entorno\n"
                                             "y ffmpeg esté disponible en su ubicación!");
        QApplication::alert(this);
    }
    else
    {
        Downloadfiles();
    }
}

void MainWindow::onFinishDowloadfiles()
{
    pictadlfiles.kill();
    pictadlfiles.terminate();
    pictadlfiles.close();
    download_count++;
    itemlist = 0;
    stdoutString.clear();
    if (download_count < 2)
    {
        downloadsAni.stop();
        ui->lbl_download->setText("<html><head/><body><p><span style=\" font-weight:600;\">Descargar</span></p></body></html>");
        ui->cmmd_download->setIcon(QIcon(":/Logos/downloadbutton.png"));
        ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
        ui->tableWidget->verticalHeader()->setDefaultAlignment(Qt::AlignVCenter);

        if (errString.isEmpty())
        {
            if (!stopped)
            {
                QString qmlmessage = "Se ha terminado de descargar los archivos";
                if (notification)
                {
                    showNotifiedMessage(qmlmessage);
                }
                else
                {
                    this->showNormal();
                    QApplication::alert(this);
                    QMessageBox::information(this, "Descarga terminada", qmlmessage);
                }
                ui->cmmd_stop->setEnabled(false);
                ui->cmmd_process->setEnabled(true);
                ui->cmmd_download->setEnabled(false);
                ui->cmmd_dlte->setEnabled(true);
            }
        }
        else
        {
            Download_Process_Error(errString);
        }
    }
}

void MainWindow::Download_Process_Error(QString error)
{
    QString qmbTitle("Error al Descargar:\n");
    QString qmbError;

    if (error.contains("WARNING: You have requested multiple formats but ffmpeg or avconv are not installed", Qt::CaseInsensitive))
    {
        qmbError = "¡No tiene instalado FFmpeg!\n"
                   "Posible que no esté bien configurado";

        ShowErrorMessage(qmbTitle, qmbError);
    }

    else if (error.contains("ERROR: unable to download video data:", Qt::CaseSensitive))
    {
        qmbError = "Ha habido un error de conexión con el servidor\n"
                   "Revise su conexión a la RED y al sitio de Picta";

        ShowErrorMessage(qmbTitle, qmbError);
    }
    else if (error.contains("ERROR: requested format not available", Qt::CaseSensitive))
    {
        qmbError = "No existe la calidad de vídeo selecciona para la descarga\n"
                   "Por favor seleccione otra y pruebe de nuevo";

        ShowErrorMessage(qmbTitle, qmbError);
    }

    else
    {
        qmbError = "¡Ha ocurrido un error inesperado!\n"
                   "Vuelva a intentarlo dentro de un rato";

        ShowErrorMessage(qmbTitle, qmbError);
    }

    ui->cmmd_process->setEnabled(true);
    ui->cmmd_download->setEnabled(true);
    ui->cmmd_stop->setEnabled(false);
    ui->cmmd_dlte->setEnabled(true);
}

QString MainWindow::CutName(QString name, int chars)
{
    if (name.length() > chars)
    {
        return name.midRef(0, chars) + "..." + withExtension(name);
    }

    return name;
}

bool MainWindow::IsNetworkConnected()
{
    bool bReturn = false;

    QList<QNetworkInterface> ifaces = QNetworkInterface::allInterfaces();
    if (!ifaces.isEmpty())
    {
        for (int i = 0; i < ifaces.size(); i++)
        {
            unsigned int flags = ifaces[i].flags();
            bool isLoopback = (bool)(flags & QNetworkInterface::IsLoopBack);
            bool isP2P = (bool)(flags & QNetworkInterface::IsPointToPoint);
            bool isRunning = (bool)(flags & QNetworkInterface::IsRunning);
            bool isUp = (bool)(flags && QNetworkInterface::IsUp);

            // If this interface isn't running, we don't care about it
            if (!isRunning)
                continue;
            // We only want valid interfaces that aren't loopback/virtual and not point to point
            if (!ifaces[i].isValid() || isLoopback || isP2P || !isUp)
                continue;

            bReturn = true;
        }
    }

    return bReturn;
}

void MainWindow::on_cmmd_stop_clicked()
{
    stopped = true;
    if (pictadlfiles.state() == 2)
    {
        pictadlfiles.kill();
        pictadlfiles.terminate();
        pictadlfiles.close();
        ui->cmmd_download->setEnabled(true);
        ui->cmmd_process->setEnabled(true);
        ui->cmmd_stop->setEnabled(false);
        ui->cmmd_dlte->setEnabled(true);
        QMessageBox::warning(this, "Error al Descargar archivos", "La descarga ha sido detenida por el usuario\n\n"
                                                                  "Puede dar clic en el botón \"Descargar\" para resumir la descarga\n\n"
                                                                  "ó dar clic en el botón \"Procesar URL\" para volver a procesar.");
        QApplication::alert(this);
    }

    if (pictadl.state() == 2)
    {
        pictadl.kill();
        pictadl.terminate();
        pictadl.close();
        ui->cmmd_download->setEnabled(false);
        ui->cmmd_process->setEnabled(true);
        ui->cmmd_stop->setEnabled(false);
        ui->cmmd_dlte->setEnabled(true);
        QMessageBox::critical(this, "Error al Procesar la URL", "Procesar URL ha sido detenido por el usuario\n\n"
                                                                "Vuelva a procesar la URL para mostrar la lista a descargar.");
        QApplication::alert(this);
    }
}

void MainWindow::on_cmmd_dlte_clicked()
{
    if (pictadlfiles.state() == 0 && pictadl.state() == 0)
    {
        stdoutString.clear();
        errString.clear();
        stopped = false;
        process_count = 0;
        download_count = 0;
        ui->lineEdit_url->setText("");
        ui->tableWidget->setRowCount(0);
        QMessageBox::information(this, "Limpiar Lista", "Se ha limpiado la lista de descarga\n\n"
                                                        "Vuelva a procesar la URL para mostrar la lista a descargar.");
        QApplication::alert(this);
        ui->cmmd_dlte->setEnabled(false);
        ui->cmmd_download->setEnabled(false);
        ui->cmmd_stop->setEnabled(false);
    }
}

void MainWindow::on_cmmd_help_clicked()
{
    information inf;
    inf.setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    inf.exec();
}

//SysTrayIcon
void MainWindow::createActions()
{
    QDir roaming(QStandardPaths::standardLocations(QStandardPaths::AppDataLocation)[0]);
    loadConfigFile(roaming);

    minimizeAction = new QAction(tr("Minimizar"), this);
    if (systray)
    {
        connect(minimizeAction, SIGNAL(triggered()), this, SLOT(hide()));
    }
    else
    {
        connect(minimizeAction, SIGNAL(triggered()), this, SLOT(MinimizeWindows()));
    }

    restoreAction = new QAction(tr("Restaurar"), this);
    connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));

    quitAction = new QAction(tr("Salir"), this);
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
}

void MainWindow::MinimizeWindows()
{
    this->setWindowState(Qt::WindowMinimized);
}

void MainWindow::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setToolTip(tr("Picta Downloader GUI"));
}

void MainWindow::showMessage()
{
    QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon();
    trayIcon->showMessage(tr("Picta Downloader GUI"), tr("GUI para descarga con picta-dl"), icon, 1000);
}

void MainWindow::showNotifiedMessage(QString message)
{
    QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon();

    QString qmbTitle("Picta Downloader GUI");

    trayIcon->showMessage(qmbTitle, message, icon, 2000);
    connect(trayIcon, &QSystemTrayIcon::messageClicked, this, &MainWindow::messageClicked);
}

void MainWindow::messageClicked()
{
    this->showNormal();
    this->raise();
    this->activateWindow();
}
void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (systray)
    {
        switch (reason)
        {
        case QSystemTrayIcon::Trigger:
            if (this->isVisible())
            {
                this->hide();
            }
            else
            {
                this->showNormal();
                this->activateWindow();
            }

        default:;
        }
    }
}

void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::WindowStateChange)
    {
        QDir roaming(QStandardPaths::standardLocations(QStandardPaths::AppDataLocation)[0]);
        loadConfigFile(roaming);

        if (isMinimized() && systray)
        {
            hide();
            if (notification)
            {
                QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon(QSystemTrayIcon::Information);

                trayIcon->showMessage(tr("Picta Downloader GUI"),
                                      tr("Minimizada a la bandeja del sistema"), icon, 100);
            }
        }
    }
}

void MainWindow::ShowErrorMessage(QString Title, QString Error)
{
    if (notification)
    {
        showNotifiedMessage(Title + Error);
    }
    else
    {
        QMessageBox::warning(this, Title, Error);
        QApplication::alert(this);
    }
}

bool MainWindow::ExistsProgram(QString program)
{
    QProcess prog;
    QString prog_stdout;
    QStringList args;
    bool exitcode;

    args << "/c" << program << "-h";

    prog.setProcessChannelMode(QProcess::MergedChannels);
    prog.setProgram("cmd.exe");
    prog.setArguments(args);
    prog.start();
    // TODO: Set/Read delay (TIMEOUT) from picta-dl-gui.conf
    delay();
    prog.kill();
    prog_stdout = prog.readAllStandardOutput();

    if (prog_stdout.contains("Usage: picta-dl [OPTIONS]", Qt::CaseSensitive) ||
        prog_stdout.contains("usage: ffmpeg [options]", Qt::CaseSensitive))
    {
        exitcode = true;
    }
    else
    {
        exitcode = false;
    }
    return exitcode;
}

void MainWindow::delay(int delaytime)
{
    QTime dieTime= QTime::currentTime().addSecs(delaytime);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}
