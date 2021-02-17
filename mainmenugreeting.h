#ifndef MAINMENUGREETING_H
#define MAINMENUGREETING_H

#include <QMainWindow>

namespace Ui {
class MainMenuGreeting;
}

class MainMenuGreeting : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainMenuGreeting(QWidget *parent = nullptr);
    ~MainMenuGreeting();

private slots:
    void on_pushButton_DownloadTool_clicked();
    void on_pushButton_UploadTool_clicked();
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::MainMenuGreeting *ui;

    class Dialog_DownloadTool* download_window = nullptr;
    class Dialog_UploadTool* upload_window = nullptr;
};

#endif // MAINMENUGREETING_H
