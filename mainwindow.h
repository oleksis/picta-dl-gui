#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QFileDialog>
#include <QStandardPaths>
#include <QToolTip>
#include <QProcess>
#include <QClipboard>
#include <QMimeData>
#include <QRegularExpression>
#include <QMovie>
#include <QtNetwork/QNetworkInterface>
#include <QSettings>
#include <QMenu>
#include <QSystemTrayIcon>
#include <QAction>
#include <QCloseEvent>
#include <QTime>
#include <QPointer>
#include "configuration.h"
#include "simplecrypt.h"
#include "information.h"
namespace Ui
{
    class MainWindow;
}

class QCheckBox;

class MainWindow : public QMainWindow
{
    Q_OBJECT

    QList<QPointer<QCheckBox>> ItemSelected;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    inline QString withExtension(const QString &fileName)
    {
        int dotIndex = fileName.lastIndexOf(".");

        if (dotIndex == -1)
        {
            return QString("");
        }

        return fileName.right(fileName.length() - dotIndex);
    }

    QString CutName(QString name, int chars);
    bool find_line(QString stringline, QString stringsearch);

protected:
    /* Virtual function of the parent class in our class
     * Overridden to change the behavior of the application,
     * That it is minimized to tray when we want
     */
    //void closeEvent(QCloseEvent * event);
    void changeEvent ( QEvent *event );

private slots:
    void on_cmmd_config_clicked();

    void on_toolButton_clicked();

    void on_bnt_clipboard_clicked();

    void on_cmmd_process_clicked();

    void onFinishGetfilenames();

    void setRefreshIcon();
    void setDownloadIcon();

    void on_cmmd_download_clicked();
    void onFinishDowloadfiles();

    void on_cmmd_stop_clicked();

    void on_cmmd_dlte_clicked();

    void on_cmmd_help_clicked();

    void messageClicked();
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void MinimizeWindows();

private:
    Ui::MainWindow *ui;

    QString pictadlDLLpath;
    QString ffmpegDLLpath;
    QString defaultDownloadpath;
    QString proxy;
    QString port;
    QString proxy_user;
    QString proxy_pass;
    QString picta_user;
    QString picta_pass;
    SimpleCrypt crypto_pass;
    quint64 crytokey = 1596880491598749;
    QString pictaGuiConfigFile;
    QString pictaConfigFile;
    QString pasteUrl;
    QProcess pictadl;
    QProcess pictadlfiles;
    QString errString;
    QString stdoutString;
    ushort process_count = 0;
    ushort download_count = 0;
    QMovie progressAni;
    QMovie downloadsAni;
    bool IsVideo;
    bool IsAudio;
    int itemlist = 0;
    int ColId = 0;
    int ColFile = 1;
    int ColVideo = 2;
    int ColAudio = 3;
    int Colsubtitle = 4;
    int ColVelocidad = 5;
    int ColProcess = 6;
    QStringList ItemOut;
    bool stopped = false;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    QAction *minimizeAction;
    QAction *maximizeAction;
    QAction *restoreAction;
    QAction *quitAction;
    bool systray;
    bool notification;
    bool envpictadl;
    bool envffmpeg;

    void configure();
    void checkExistenceOfMainProcess();
    void createConfigFile(QDir &roaming);
    bool loadConfigFile(QDir &roaming);
    void saveConfigFile();
    bool PasteFromClipboard();
    void get_filename();
    void Downloadfiles();
    void URL_Process_Error(QString error);
    void Download_Process_Error(QString error);
    void ShowErrorMessage(QString Title, QString Error);
    bool IsNetworkConnected();
    bool ExistsProgram(QString program);
    void delayProcess(QProcess &process);
    QStringList GetArguments();
    QString GetSelectedItems();
    QStringList FixedArgs;
    QString playlistitems;

    void createActions();
    void createTrayIcon();
    void showMessage();
    void showNotifiedMessage(QString message);
};

#endif // MAINWINDOW_H
