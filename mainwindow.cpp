#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
                                          ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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
}

bool MainWindow::loadConfigFile(QDir &roaming)
{
    QFile configFile(roaming.absolutePath().append("/picta-dl-gui.conf"));
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
        cproxy_user =  settings.value("uproxy").toString();
        cproxy_pass =  settings.value("pproxy").toString();
        cpicta_user  =  settings.value("upicta").toString();
        cpicta_pass  =  settings.value("ppicta").toString();

        settings.endGroup();

        QChar backslash(92);
        ui->lineEdit_Location->setText(filePath.replace("/", backslash));

        defaultDownloadpath = filePath;
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
    QFile configFile(roaming.absolutePath().append("/picta-dl-gui.conf"));
    QFileInfo configInfo(configFile);
    QSettings settings(configInfo.absoluteFilePath(), QSettings::IniFormat);

    if (!settings.isWritable())
        return;

    QString savedpath, cproxy, cport, cproxy_user ,
                       cproxy_pass , cpicta_user , cpicta_pass;
    savedpath = defaultDownloadpath;

    settings.beginGroup("General");

    settings.setValue("savedPath", savedpath);
    settings.setValue("proxy", cproxy);
    settings.setValue("port", cport);
    settings.setValue("uproxy", cproxy_user);
    settings.setValue("pproxy", cproxy_pass);
    settings.setValue("upicta", cpicta_user);
    settings.setValue("ppicta", cpicta_pass);
    
    settings.endGroup();

    //Creating default picta.conf

    QFile configPictaFile(roaming.absolutePath().append("\\picta-dl.conf"));
    if (!configPictaFile.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out_picta_conf(&configPictaFile);

    QString PictaFormat = ("-o \"%(title)s.%(ext)s\"");
    QString ffmpeg_conf = (ffmpegDLLpath);
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
    QFile configFile(QString(QStandardPaths::standardLocations(QStandardPaths::AppDataLocation)[0]).append("/picta-dl-gui.conf"));
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
        pictadl.setProgram(pictadlDLLpath);

        QStringList args;
        args << "--config-location" << picta_conf << "--abort-on-error"
             << "--socket-timeout"
             << "10"
             << "--retries"
             << "6";
        args << "--get-filename";

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
            args << "--proxy" << proxy + ":" + port;
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

                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 0, new QTableWidgetItem(ShortName));
                ui->tableWidget->item(ui->tableWidget->rowCount() - 1, 0)->setToolTip(ListItem);
                ui->tableWidget->item(ui->tableWidget->rowCount() - 1, 0)->setTextAlignment(Qt::AlignLeft);
                ui->tableWidget->item(ui->tableWidget->rowCount() - 1, 0)->setTextAlignment(Qt::AlignVCenter);

                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, ColVideo, new QTableWidgetItem("➖"));
                ui->tableWidget->item(ui->tableWidget->rowCount() - 1, 1)->setTextAlignment(Qt::AlignCenter);

                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, ColAudio, new QTableWidgetItem("➖"));
                ui->tableWidget->item(ui->tableWidget->rowCount() - 1, 2)->setTextAlignment(Qt::AlignCenter);

                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, Colsubtitle, new QTableWidgetItem("➖"));
                ui->tableWidget->item(ui->tableWidget->rowCount() - 1, 3)->setTextAlignment(Qt::AlignCenter);

                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, ColVelocidad, new QTableWidgetItem("➖"));
                ui->tableWidget->item(ui->tableWidget->rowCount() - 1, 4)->setTextAlignment(Qt::AlignCenter);

                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, ColProcess, new QTableWidgetItem("➖"));
                ui->tableWidget->item(ui->tableWidget->rowCount() - 1, 5)->setTextAlignment(Qt::AlignCenter);
            }

        } while (!listfile.atEnd());

        playlistfile.close();
        progressAni.stop();
        ui->cmmd_process->setIcon(QIcon(":/Logos/refreshbutton.png"));
        ui->lbl_process->setText("<html><head/><body><p><span style=\" font-weight:600;\">Procesar URL</span></p></body></html>");

        if (!ListItem.contains("ERROR:", Qt::CaseSensitive))
        {
            if (!stopped)
            {
                QMessageBox::information(this, "Procesado terminado", "Se ha terminado de procesar la lista de descarga");

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
    QString qmbTitle("Error al Procesar la URL");

    if (error.contains("ERROR: This playlist is only available for registered users", Qt::CaseSensitive))
    {
        QMessageBox::warning(this, qmbTitle, "Las listas de reproducción solo está disponible para usuarios registrados\n\n"
                                             "Revise que tiene configurado correctamente su usuario y contraseña del sitio de picta.");
        QApplication::alert(this);
    }

    if (error.contains("ERROR: Playlist no exists!", Qt::CaseSensitive))
    {
        QMessageBox::warning(this, qmbTitle, "No existe la lista de reproducción que intenta procesar\n\n"
                                             "Revise que aún existe la lista de reproducción en el sitio.");
        QApplication::alert(this);
    }

    if (error.contains("ERROR: no suitable InfoExtractor for URL", Qt::CaseSensitive))
    {
        QMessageBox::warning(this, qmbTitle, "Es incorrecta la URL que intenta procesar\n\n"
                                             "Revise que este correcta en el sitio:\n\n"
                                             "https://www.picta.cu/");
        QApplication::alert(this);
    }

    if (error.contains("ERROR: Unable to download JSON metadata:", Qt::CaseSensitive))
    {
        QMessageBox::critical(this, qmbTitle, "Ha habido un error de conexión con el servidor\n\n"
                                              "Revise que esta conectado a la red o que el sitio de Picta este disponible.");
        QApplication::alert(this);
    }

    if (error.contains("ERROR: Unable to download JSON metadata: <urlopen error [SSL: CERTIFICATE_VERIFY_FAILED]", Qt::CaseSensitive))
    {
        QMessageBox::critical(this, qmbTitle, "Ha habido un error de certificado SSL de su sistema\n\n"
                                              "Si tiene una versión recientemente de Windows 10 (2004)\n"
                                              "es posible que tenga que actualizar sus sistema con los últimos parches de seguridad. Vea en la ayuda los F.A.Q");
        QApplication::alert(this);
    }

    if (error.contains("ERROR: Failed to download MPD manifest:", Qt::CaseSensitive))
    {
        QMessageBox::critical(this, qmbTitle, "Ha habido un error de conexión con el servidor\n\n"
                                              "Revise que esta conectado a la red o que el sitio de Picta este disponible.");
        QApplication::alert(this);
    }

    if (error.contains("ERROR: Cannot find video!", Qt::CaseSensitive) || error.isEmpty())
    {
        QMessageBox::warning(this, qmbTitle, "¡No se ha encontrado el vídeo en el sitio !\n\n"
                                             "Revise que este correcta la URL en el sitio.");
        QApplication::alert(this);
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
        QMessageBox::critical(this, qmbTitle, "¡NO ESTA CONECTADO A LA RED!\n\n"
                                              "Revise su conexión de RED/HUB/Router/WIFI o Datos!");
        QApplication::alert(this);
    }
    else if (picta_pass.isEmpty() || picta_user.isEmpty())
    {

        QMessageBox::warning(this, qmbTitle, "¡Debe configurar su usuario y contraseña de Picta!\n\n"
                                             "Revise que tenga configurado el usuario y contraseña del sitio de Picta.");
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
    ui->cmmd_process->setEnabled(false);
    //Animated Process list
    downloadsAni.setFileName(":/Logos/download_animated.gif");
    downloadsAni.start();
    ui->lbl_download->setText("<html><head/><body><p><span style=\" font-weight:600;\">Descargando...</span></p></body></html>");
    connect(&downloadsAni, SIGNAL(frameChanged(int)), this, SLOT(setDownloadIcon()));

    //Prepare arguments
    QStringList args;
    args << "--config-location" << picta_conf << "--abort-on-error"
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

    //    if (!ui->chckBox_Onlyaudio->isChecked())
    //    {
    //        if (ui->radBnt_Alta->isChecked())
    //        {
    //            args << "-f 0+5";
    //        } else if (ui->radBnt_Media->isChecked()) {
    //            args << "-f 2+5";
    //        } else {
    //            args << "-f 4+5";
    //        }
    //     }

    if (!proxy.isEmpty())
    {
        args << "--proxy" << proxy + ":" + port;
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
    pictadlfiles.setProgram(pictadlDLLpath);
    pictadlfiles.start();
    ui->cmmd_download->setEnabled(false);
    ui->cmmd_stop->setEnabled(true);
    ui->cmmd_dlte->setEnabled(false);

    //Debug() << "Arguments Download: " << args;

    connect(&pictadlfiles, &QProcess::readyReadStandardOutput, this, [&]() {
        stdoutString = pictadlfiles.readAllStandardOutput();

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
        QMessageBox::critical(this, qmbTitle, "¡NO ESTA CONECTADO A LA RED!\n\n"
                                              "Revise su conexión de RED/HUB/Router/WIFI o Datos!");
        QApplication::alert(this);
    }
    else if (picta_pass.isEmpty() || picta_user.isEmpty())
    {

        QMessageBox::warning(this, qmbTitle, "¡Debe configurar su usuario y contraseña de Picta!\n\n"
                                             "Revise que tenga configurado el usuario y contraseña del sitio de Picta.");
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
                QMessageBox::information(this, "Descarga terminada", "Se ha terminado de descargar los archivos");
                ui->cmmd_stop->setEnabled(false);
                ui->cmmd_process->setEnabled(true);
                ui->cmmd_download->setEnabled(false);
                ui->cmmd_dlte->setEnabled(true);
            }
        }
        else
        {
            if (errString.contains("ERROR: unable to download video data:", Qt::CaseSensitive))
            {
                QMessageBox::critical(this, "Error al Descargar archivos", "Ha habido un error de conexión con el servidor\n\n"
                                                                           "Revise que esta conectado a la red "
                                                                           "o que el sitio de Picta este disponible.");
                QApplication::alert(this);
                errString.clear();
                ui->cmmd_stop->setEnabled(false);
                ui->cmmd_process->setEnabled(true);
                ui->cmmd_dlte->setEnabled(true);
            }
        }
    }
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
        ui->cmmd_download->setEnabled(false);
        ui->cmmd_process->setEnabled(true);
        ui->cmmd_stop->setEnabled(false);
        ui->cmmd_dlte->setEnabled(true);
        QMessageBox::critical(this, "Error al Descargar archivos", "La descarga ha sido detenida por el usuario\n\n"
                                                                   "Vuelva a procesar la URL para volver a descargar.");
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
