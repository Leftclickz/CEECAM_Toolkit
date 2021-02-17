#ifndef DIALOG_DOWNLOADTOOL_H
#define DIALOG_DOWNLOADTOOL_H

#include <QDialog>
#include <QProcess>
#include <QCloseEvent>

namespace Ui {
class dialog_downloadtool;
}

#define CC_TK_VERSION "f50bd3f173113836cdcdf2486501e3a8905989eb"

#include "google/cloud/storage/client.h"
namespace gcs = google::cloud::storage;

class Dialog_DownloadTool : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_DownloadTool(QWidget *parent = nullptr);
    ~Dialog_DownloadTool();

    void LoadPatterns();
    void ShutdownProxy();
    void ConvertStringToTM(std::tm& timepoint, std::string data, std::string pattern);
    void AddToDisplayLog(QString val);

    void DisconnectAll();

    QString GetPatternText();

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
    Ui::dialog_downloadtool *ui;
    bool m_ActiveGCSDownloadThread = false;

public:
    bool gacSet = false, conSet = false, proxySet = false;
    class ArgumentMap* args;

    google::cloud::StatusOr<gcs::Client> clientHandle;
    std::map<std::string, std::string> patterns;
    QProcess* proxyConnector;
};

#endif // DIALOG_DOWNLOADTOOL_H
