#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define CC_TK_VERSION "f50bd3f173113836cdcdf2486501e3a8905989eb"

#include "google/cloud/storage/client.h"
namespace gcs = google::cloud::storage;

#include <QMainWindow>
#include <QProcess>
#include <QCloseEvent>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void LoadPatterns();
    void ConvertStringToTM(std::tm& timepoint, std::string data, std::string pattern);

    void AddToDisplayLog(QString val);

    QString GetPatternText();

    void ShutdownProxy();

private slots:
    void on_pushButton_LoadTable_clicked();

    void on_pushButton_Download_clicked();

    void on_lineEdit_StartDate_textChanged(const QString &arg1);

    void on_lineEdit_EndDate_textChanged(const QString &arg1);

    void on_lineEdit_InputDir_textChanged(const QString &arg1);

    void on_lineEdit_OutputDir_textChanged(const QString &arg1);

    void on_lineEdit_StartTime_textChanged(const QString &arg1);

    void on_lineEdit_EndTime_textChanged(const QString &arg1);

    void on_checkBox_Recursive_stateChanged(int arg1);

    void on_checkBox_DailyFolders_stateChanged(int arg1);

    void HandleGCSDownload();

    void HandleGCSUpdateText(QString text);

    void closeEvent(QCloseEvent *event) override;


private:
    Ui::MainWindow *ui;
    bool m_ActiveGCSDownloadThread = false;

public:
    bool gacSet = false, conSet = false, proxySet = false;
    class ArgumentMap* args;

    google::cloud::StatusOr<gcs::Client> clientHandle;
    std::map<std::string, std::string> patterns;
    QProcess* proxyConnector;



};
#endif // MAINWINDOW_H
