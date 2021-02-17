#include "mainmenugreeting.h"
#include "ui_mainmenugreeting.h"

#include "dialog_downloadtool.h"
#include "dialog_uploadtool.h"

MainMenuGreeting::MainMenuGreeting(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainMenuGreeting)
{
    ui->setupUi(this);
    setWindowTitle("CEECAM Toolkit");
}

MainMenuGreeting::~MainMenuGreeting()
{
    delete ui;

    if (download_window != nullptr)
        delete download_window;
    if (upload_window != nullptr)
        delete upload_window;
}

void MainMenuGreeting::on_pushButton_DownloadTool_clicked()
{
    if (download_window == nullptr)
        download_window = new Dialog_DownloadTool(this);

    download_window->show();
}

void MainMenuGreeting::on_pushButton_UploadTool_clicked()
{
    if (upload_window == nullptr)
        upload_window = new Dialog_UploadTool(this);

    upload_window->show();
}

void MainMenuGreeting::closeEvent(QCloseEvent *event)
{
    if (download_window != nullptr)
        download_window->DisconnectAll();

    event->accept();
}
