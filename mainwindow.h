#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define CC_TK_VERSION "f50bd3f173113836cdcdf2486501e3a8905989eb"

#include "google/cloud/storage/client.h"
namespace gcs = google::cloud::storage;

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void AttemptGCSDownload(QString InputDir, QString OutputDir);

    void LoadPatterns();
    void ConvertStringToTM(std::tm& timepoint, std::string data, std::string pattern);

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

private:
    Ui::MainWindow *ui;
    class ArgumentMap* args;

public:
    bool gacSet = false;
    google::cloud::StatusOr<gcs::Client> clientHandle;

    std::map<std::string, std::string> patterns;



};
#endif // MAINWINDOW_H
