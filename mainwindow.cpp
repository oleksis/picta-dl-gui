#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent, bool isDarkTheme) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    isDark = isDarkTheme;
    pictaGuiConfigFile = QString("picta-dl-gui.conf");
    pictaConfigFile = QString("picta-dl.conf");
    configure();
    CenterWidgets(this);
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
    QDir appDirPath(QCoreApplication::applicationDirPath());
    QString defaultpath(QStandardPaths::standardLocations(QStandardPaths::DownloadLocation)[0]);

    if (!loadConfigFile(appDirPath))
        createConfigFile(appDirPath);

#ifdef Q_OS_WIN
    pictadlDLLpath = appDirPath.absolutePath().append("\\picta-dl.exe");
    ffmpegDLLpath = appDirPath.absolutePath().append("\\ffmpeg.exe");
    QFile pictadlExe(pictadlDLLpath);

    if (!envpictadl && !pictadlExe.exists())
        ui->cmmd_help->click();

#endif
    defaultDownloadpath = defaultpath;
#ifdef Q_OS_WIN
    QChar backslash(92);
    ui->lineEdit_Location->setText(defaultpath.replace("/", backslash));
#else
    ui->lineEdit_Location->setText(defaultpath);
#endif
    crypto_pass.setKey(crytokey);
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

bool MainWindow::loadConfigFile(QDir &appDirPath)
{
    QFile configFile(appDirPath.absolutePath().append("/") + pictaGuiConfigFile);
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
#ifdef Q_OS_WIN
        QChar backslash(92);
        ui->lineEdit_Location->setText(filePath.replace("/", backslash));
#else
        ui->lineEdit_Location->setText(filePath);
#endif
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

void MainWindow::createConfigFile(QDir &appDirPath)
{
    QFile configFile(appDirPath.absolutePath().append("/") + pictaGuiConfigFile);
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
    QFile configPictaFile(appDirPath.absolutePath().append("/") + pictaConfigFile);

    if (!configPictaFile.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out_picta_conf(&configPictaFile);
    QString PictaFormat = ("-o \"%(title)s.%(ext)s\"");
#ifdef Q_OS_WIN
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
#else
    out_picta_conf << "# Lines starting with # are comments\n\n"
                   << "# Format video output <video>.<extension>\n"
                   << PictaFormat.append("\n\n");
#endif
    configPictaFile.close();
}

void MainWindow::saveConfigFile()
{
    QDir appDirPath(QCoreApplication::applicationDirPath());
    QFile configFile(appDirPath.absolutePath().append("/") + pictaGuiConfigFile);
    QFileInfo configInfo(configFile);
    QSettings settings(configInfo.absoluteFilePath(), QSettings::IniFormat);

    if (!settings.isWritable())
        return;

    QString savedpath;
    savedpath = defaultDownloadpath;
    crypto_pass.setKey(crytokey);
    QString crytopass_picta = crypto_pass.encryptToString(picta_pass), crytopass_proxy;

    if (!proxy_pass.isEmpty())
        crytopass_proxy = crypto_pass.encryptToString(proxy_pass);

    if (!picta_pass.isEmpty())
        crytopass_picta = crypto_pass.encryptToString(picta_pass);

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

    QDir appDirPath(QCoreApplication::applicationDirPath());
    loadConfigFile(appDirPath);
    defaultDownloadpath = downloadFolder;
#ifdef Q_OS_WIN
    QChar backslash(92);
    ui->lineEdit_Location->setText(downloadFolder.replace("/", backslash));
#else
    ui->lineEdit_Location->setText(downloadFolder);
#endif
    saveConfigFile();
}

bool MainWindow::PasteFromClipboard()
{
    const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();
    QString CopyTextClip = clipboard->text();
    QString qmbTitle("Error al Pegar");
    QString infoText("Sólo puede pegar Urls que sean del sitio de Picta o Youtube ejemplo:\n\n"
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
        return false;
}

void MainWindow::on_bnt_clipboard_clicked()
{
    QString qmbTitle("Error al Pegar");
    QString infoText("Sólo puede pegar Urls que sean del sitio de Picta o Youtube ejemplo:\n\n"
                     "https://www.picta.cu/medias/... https://www.youtube.com/...");
    IsYoutubeUrl = false;

    if (PasteFromClipboard())
    {
        QRegularExpression regex("(\b(?:https?|http)://)?[-A-Za-z0-9+&@#/%?=~_|!:,.;]+[-A-Za-z0-9+&@#/%=~_|]");

        if (regex.match(pasteUrl).hasMatch())
        {
            // Picta puede descargar desde /medias/ | /movie/
            QString pictaurl("https://www.picta.cu/m");
            QString youtube_url("https://www.youtube.com/");
            QString youtu_be_url("https://youtu.be/");
            QString m_youtube_url("https://m.youtube.com/");

            if (find_line(pasteUrl, youtube_url) || find_line(pasteUrl, youtu_be_url) || find_line(pasteUrl, m_youtube_url))
                IsYoutubeUrl = true;

            if (find_line(pasteUrl, pictaurl) || IsYoutubeUrl)
                ui->lineEdit_url->setText(pasteUrl);
            else
            {
                QMessageBox::information(this, qmbTitle, QString("No es una URL válida de Picta o Youtube\n\n") + infoText);
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
        return true;

    return false;
}

void MainWindow::get_filename()
{
    QString qmbTitle("Error al Pegar");
    QString infoText("Sólo puede pegar Urls que sean del sitio de Picta o Youtube ejemplo:\n\n"
                     "https://www.picta.cu/medias/... https://www.youtube.com/...");

    if (!ui->lineEdit_url->text().isEmpty())
    {
        QDir appDirPath(QCoreApplication::applicationDirPath());
        QString picta_conf = appDirPath.absolutePath().append("/picta-dl.conf");
        QString playlist = appDirPath.absolutePath().append("/playlist.txt");
        ui->tableWidget->setRowCount(0);
        process_count = 0;
        FixedArgs.clear();
        //Animated Process list
        progressAni.setFileName(isDark ? ":/Logos/refresh_rotate_dark.gif" : ":/Logos/refresh_rotate_light.gif");
        progressAni.start();
        ui->lbl_process->setText("<html><head/><body><p><span style=\" font-weight:600;\">Procesando URL...</span></p></body></html>");
        connect(&progressAni, SIGNAL(frameChanged(int)), this, SLOT(setRefreshIcon()));
        //Prepare process for url list
        pictadl.setProcessChannelMode(QProcess::MergedChannels);
        QStringList args;
#ifdef Q_OS_WIN

        if (envpictadl)
            pictadl.setProgram("picta-dl.exe");
        else
            pictadl.setProgram(pictadlDLLpath);

        if (envffmpeg)
        {
            args << "--ignore-config"
                 << "-o"
                 << "%(title)s.%(ext)s";
        }
        else
            args << "--config-location" << picta_conf;

#else
        pictadl.setProgram("picta-dl");
        args << "--config-location" << picta_conf;
#endif
        args << "--abort-on-error"
             << "--socket-timeout"
             << "10"
             << "--retries"
             << "6"
             << "--get-filename";
        FixedArgs = GetArguments();
        args << FixedArgs;

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

        if (!IsYoutubeUrl)
            args << "-u" << picta_user << "-p" << picta_pass;

        args << pasteUrl;
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
    playlistitems.clear();
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableWidget->verticalHeader()->setDefaultAlignment(Qt::AlignVCenter);

    if (process_count < 2)
    {
        //Read processed list
        QDir appDirPath(QCoreApplication::applicationDirPath());
        QFile playlistfile(appDirPath.absolutePath().append("/playlist.txt"));
        playlistfile.open(QIODevice::ReadWrite | QIODevice::Text);
        QTextStream listfile(&playlistfile);
        QString ListItem, ShortName;
        int LastRow;
        ItemSelected.clear();

        do
        {
            ListItem = listfile.readLine();

            if (!ListItem.contains("ERROR:", Qt::CaseSensitive) && !ListItem.isEmpty())
            {
                ShortName = CutName(ListItem, 23);
                ui->tableWidget->insertRow(ui->tableWidget->rowCount());
                LastRow = ui->tableWidget->rowCount() - 1;
                QPointer<QCheckBox> Id = new QCheckBox(this);                   // Create a QCheckbox pointer
                QWidget *checkBoxWidget = new QWidget();                        // Create a QWiget container
                QHBoxLayout *layoutCheckBox = new QHBoxLayout(checkBoxWidget);  // Create a Horizontal layout
                layoutCheckBox->addWidget(Id);                                  // Add Widget to layout
                layoutCheckBox->setAlignment(Qt::AlignCenter);                  // Center the checkbox
                layoutCheckBox->setContentsMargins(0, 0, 0, 0);                 // Set the zero padding
                ui->tableWidget->setCellWidget(LastRow, ColId, checkBoxWidget); // Add a CheckBox with Layout to the Table
                ItemSelected.append(Id);                                        // Add to list Pointer of QCheckbox
                ui->tableWidget->setItem(LastRow, ColFile, new QTableWidgetItem(ShortName));
                ui->tableWidget->item(LastRow, ColFile)->setToolTip(ListItem);
                ui->tableWidget->item(LastRow, ColFile)->setTextAlignment(Qt::AlignLeft);
                ui->tableWidget->item(LastRow, ColFile)->setTextAlignment(Qt::AlignVCenter);
                ui->tableWidget->setItem(LastRow, ColVideo, new QTableWidgetItem("➖"));
                ui->tableWidget->item(LastRow, ColVideo)->setTextAlignment(Qt::AlignCenter);
                ui->tableWidget->setItem(LastRow, ColAudio, new QTableWidgetItem("➖"));
                ui->tableWidget->item(LastRow, ColAudio)->setTextAlignment(Qt::AlignCenter);
                ui->tableWidget->setItem(LastRow, Colsubtitle, new QTableWidgetItem("➖"));
                ui->tableWidget->item(LastRow, Colsubtitle)->setTextAlignment(Qt::AlignCenter);
                ui->tableWidget->setItem(LastRow, ColVelocidad, new QTableWidgetItem("➖"));
                ui->tableWidget->item(LastRow, ColVelocidad)->setTextAlignment(Qt::AlignCenter);
                ui->tableWidget->setItem(LastRow, ColProcess, new QTableWidgetItem("➖"));
                ui->tableWidget->item(LastRow, ColProcess)->setTextAlignment(Qt::AlignCenter);
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
                    showNotifiedMessage(qmlmessage);
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
            URL_Process_Error(ListItem);
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
    QDir appDirPath(QCoreApplication::applicationDirPath());
    loadConfigFile(appDirPath);

    if (!IsNetworkConnected())
    {
        QMessageBox::critical(this, qmbTitle, "¡No está conectado a la Red!\n\n"
                              "Revise su conexión de RED/HUB/Router/WIFI o Datos!");
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
        get_filename();
}

void MainWindow::setRefreshIcon()
{
    ui->cmmd_process->setIcon(QIcon(progressAni.currentPixmap()));
}

void MainWindow::Downloadfiles()
{
    QDir appDirPath(QCoreApplication::applicationDirPath());
    QString picta_conf = appDirPath.absolutePath().append("/picta-dl.conf");
    download_count = 0;
    ui->cmmd_process->setEnabled(false);
    //Animated Process list
    downloadsAni.setFileName(isDark ? ":/Logos/download_button_dark.gif" : ":/Logos/download_button_light.gif");
    downloadsAni.start();
    ui->lbl_download->setText("<html><head/><body><p><span style=\" font-weight:600;\">Descargando...</span></p></body></html>");
    connect(&downloadsAni, SIGNAL(frameChanged(int)), this, SLOT(setDownloadIcon()));
    //Prepare arguments
    QStringList args;
#ifdef Q_OS_WIN

    if (envpictadl)
        pictadlfiles.setProgram("picta-dl.exe");
    else
        pictadlfiles.setProgram(pictadlDLLpath);

    if (envffmpeg)
    {
        args << "--ignore-config"
             << "-o"
             << "%(title)s.%(ext)s";
    }
    else
        args << "--config-location" << picta_conf;

#else
    pictadlfiles.setProgram("picta-dl");
    args << "--config-location" << picta_conf;
#endif
    args << "--abort-on-error"
         << "--socket-timeout"
         << "10"
         << "--retries"
         << "6"
         << "--newline"
         << FixedArgs;

    if (!stopped)
        playlistitems = GetSelectedItems();

    if (!playlistitems.isEmpty())
        args << "--playlist-items" << playlistitems;

    if (ui->chckBox_Subt->isChecked())
        args << "--write-sub";

    if (!args.contains("--audio-quality"))
    {
        VideoQuality.clear();

        if (ui->radBnt_Alta->isChecked())
        {
            args << "-f";

            if (!IsYoutubeUrl)
                args << "bestvideo+bestaudio";
            else
                args << "mp4/webm/bestvideo+m4a/webm/bestaudio";

            VideoQuality = "Alta";
        }
        else if (ui->radBnt_Media->isChecked())
        {
            args << "-f";

            if (!IsYoutubeUrl)
                args << "2+5/1+bestaudio/best";
            else
                args << "[height <? 720]+m4a/webm/bestaudio";

            VideoQuality = "Media";
        }
        else
        {
            args << "-f";

            if (!IsYoutubeUrl)
                args << "4+5/2+bestaudio/best";
            else
                args << "[height <? 480]+m4a/webm/bestaudio";

            VideoQuality = "Baja";
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

    if (!IsYoutubeUrl)
        args << "-u" << picta_user << "-p" << picta_pass;

    args << pasteUrl;
    QString filePath = ui->lineEdit_Location->text();
#ifdef Q_OS_WIN
    QChar backslash(92);
    filePath.append(backslash);
#else
    filePath.append("/");
#endif
    args << "-o" << filePath + "%(title)s.%(ext)s";
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
    connect(&pictadlfiles, &QProcess::readyReadStandardOutput, this, [&]()
    {
        stdoutString = pictadlfiles.readAllStandardOutput();

        if (stdoutString.contains("Writing video subtitles", Qt::CaseSensitive))
        {
            ui->tableWidget->item(itemlist, Colsubtitle)->setText("✔");
            ui->tableWidget->item(itemlist, Colsubtitle)->setTextColor(QColor(0, 170, 0));
            ui->tableWidget->item(itemlist, Colsubtitle)->setTextAlignment(Qt::AlignCenter);
        }

        if (stdoutString.contains("WARNING: video doesn't have subtitles", Qt::CaseSensitive) || stdoutString.contains("WARNING: unable to download video subtitles", Qt::CaseSensitive))
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
        else if (stdoutString.contains("has already been downloaded", Qt::CaseSensitive) && (stdoutString.contains(".mp4") || stdoutString.contains("f4.webm") || stdoutString.contains(".webm")))
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
        else if (stdoutString.contains("[download] Destination:", Qt::CaseSensitive) && (stdoutString.contains(".m4a") || stdoutString.contains("f5.webm") || stdoutString.contains(".webm")))
        {
            IsVideo = false;
            IsAudio = true;
        }

        if (stdoutString.contains("[download] 100%", Qt::CaseSensitive) && IsVideo && !IsAudio)
            IsVideo = false;

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

            if (IsYoutubeUrl && VideoQuality == "Alta")
                itemlist++;
        }

        if (stdoutString.contains("[ffmpeg] Merging formats", Qt::CaseSensitive))
        {
            ui->tableWidget->item(itemlist, ColProcess)->setText("Procesando...");
            ui->tableWidget->item(itemlist, ColProcess)->setTextColor(QColor(170, 0, 0));
            ui->tableWidget->item(itemlist, ColProcess)->setTextAlignment(Qt::AlignCenter);
        }

        if (stdoutString.contains("Deleting original file", Qt::CaseSensitive) && (stdoutString.contains("m4a", Qt::CaseSensitive) || stdoutString.contains("f5.webm") || stdoutString.contains(".webm")))
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
            errString = stdoutString;
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
    QDir appDirPath(QCoreApplication::applicationDirPath());
    loadConfigFile(appDirPath);

    if (!IsNetworkConnected())
    {
        QMessageBox::critical(this, qmbTitle, "¡No está conectado a la Red!\n\n"
                              "Revise su conexión de RED/HUB/Router/WIFI o Datos!");
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
        Downloadfiles();
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
                    showNotifiedMessage(qmlmessage);
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
            Download_Process_Error(errString);
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
        qmbError = "No existe el formato de vídeo seleccionado\n"
                   "Seleccione otra calidad o sólo audio";
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
    int dotIndex = name.lastIndexOf(".");

    if (dotIndex == -1)
        return name;

    QString fileName = name.mid(0, dotIndex);

    if (fileName.length() > chars)
        return name.mid(0, chars) + "..." + withExtension(name);

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
            bool isLoopback = bool (flags & QNetworkInterface::IsLoopBack);
            bool isP2P =  bool (flags & QNetworkInterface::IsPointToPoint);
            bool isRunning =  bool (flags & QNetworkInterface::IsRunning);
            bool isUp =  bool (flags & QNetworkInterface::IsUp);

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
    QDir appDirPath(QCoreApplication::applicationDirPath());
    loadConfigFile(appDirPath);
    minimizeAction = new QAction(tr("Minimizar"), this);

    if (systray)
        connect(minimizeAction, SIGNAL(triggered()), this, SLOT(hide()));
    else
        connect(minimizeAction, SIGNAL(triggered()), this, SLOT(MinimizeWindows()));

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
                    this->hide();
                else
                {
                    this->showNormal();
                    this->activateWindow();
                }

                break;

            default:
                ;
        }
    }
}

void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::WindowStateChange)
    {
        QDir appDirPath(QCoreApplication::applicationDirPath());
        loadConfigFile(appDirPath);

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
        showNotifiedMessage(Title + Error);
    else
    {
        QMessageBox::warning(this, Title, Error);
        QApplication::alert(this);
    }
}

bool MainWindow::ExistsProgram(QString program)
{
    bool exitcode = false;
    QString checkprogram = program;
#ifdef Q_OS_WIN
    QProcess prog;
    QString prog_stdout;
    QStringList args;
    args << "/c" << checkprogram << "-h";
    prog.setProcessChannelMode(QProcess::MergedChannels);
    prog.setProgram("cmd.exe");
    prog.setArguments(args);
    prog.start();
    delayProcess(prog);
    prog.kill();
    prog_stdout = prog.readAllStandardOutput();

    if (prog_stdout.contains("Usage: picta-dl [OPTIONS]", Qt::CaseSensitive)
        || prog_stdout.contains("usage: ffmpeg [options]", Qt::CaseSensitive))
        exitcode = true;

#else
    // TODO: Check picta-dl and ffmpeg exist ?!
    exitcode = true;
#endif
    return exitcode;
}

void MainWindow::delayProcess(QProcess &process)
{
    while (process.state() == QProcess::Running)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

QStringList MainWindow::GetArguments()
{
    QStringList args;

    if (!ui->chckBox_Playlist->isChecked())
        args << "--no-playlist";
    else
        args << "--yes-playlist";

    if (ui->chckBox_Onlyaudio->isChecked())
    {
        args << "-x"
             << "--audio-quality"
             << "0";

        if (IsYoutubeUrl)
            args << "-f" << "m4a/webm/bestaudio";
    }
    else if (IsYoutubeUrl)
        args << "-f" << "mp4/webm/bestvideo+m4a/webm/bestaudio";

    return args;
}

QString MainWindow::GetSelectedItems()
{
    QString args;
    QString SelectItems;
    int item = 0;
    bool ischeckeditem = false;

    for (int i = 0; i < ui->tableWidget->rowCount(); ++i)
    {
        if (ItemSelected.at(i)->isChecked())
        {
            ischeckeditem = true;
            item = i + 1;
            SelectItems.append(QString::number(item) + ",");
        }
    }

    if (ischeckeditem)
    {
        for (int i = 0; i < ui->tableWidget->rowCount(); ++i)
        {
            if (!ItemSelected.at(i)->isChecked())
            {
                ui->tableWidget->removeRow(i);
                delete ItemSelected.at(i);
                ItemSelected.removeAt(i);
                i = -1;
            }
        }
    }

    args = SelectItems.mid(0, SelectItems.length() - 1);
    return args;
}
