#ifndef DIALOG_UPLOADTOOL_H
#define DIALOG_UPLOADTOOL_H

#include <QDialog>
#include <QFileDialog>
#include <QCloseEvent>

namespace Ui {
class Dialog_UploadTool;
}

class Dialog_UploadTool : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_UploadTool(QWidget *parent = nullptr);
    ~Dialog_UploadTool();

    void AddToDisplayLog(QString val);
    bool ConfirmPattern(QString pattern);


private slots:
    void on_checkBox_Recursive_stateChanged(int arg1);
    void on_lineEdit_AuthServer_textChanged(const QString &arg1);
    void on_lineEdit_EndServer_textChanged(const QString &arg1);
    void on_lineEdit_SourceDir_textChanged(const QString &arg1);
    void on_lineEdit_Pattern_textChanged(const QString &arg1);
    void on_pushButton_TestInput_clicked();
    void on_pushButton_SelectSourceDir_clicked();
    void closeEvent(QCloseEvent *event);

    void HandleUploadComplete();
    void HandleTestComplete();
    void UpdateDisplayText(QString val);
    bool PassedDirectoryChecks();
    bool PassedServerChecks();

    void on_pushButton_Upload_clicked();

    void on_lineEdit_Password_textEdited(const QString &arg1);

    void on_lineEdit_Username_textEdited(const QString &arg1);

public:
    class ArgumentMap* args;

private:
    Ui::Dialog_UploadTool *ui;

    bool m_ActiveUploadThread = false;
    static class CurlUploadThread *m_WorkerThread;

};

#endif // DIALOG_UPLOADTOOL_H
