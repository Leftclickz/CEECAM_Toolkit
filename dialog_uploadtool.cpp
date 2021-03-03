#include "dialog_uploadtool.h"
#include "ui_dialog_uploadtool.h"

#include <QJsonObject>
#include <QJsonDocument>
#include <QTextCodec>

#include "argumentmap.h"
#include "curluploadthread.h"
#include "aetl.h"

#include <curl/curl.h>

CurlUploadThread* Dialog_UploadTool::m_WorkerThread = nullptr;

Dialog_UploadTool::Dialog_UploadTool(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_UploadTool)
{
    ui->setupUi(this);
    setWindowTitle("CEECAM Toolkit - Upload Dialog");

    //Create our argument map and parse our argument file
    args = new ArgumentMap();
    args->AddDefaults("default_upload_args.txt");

    //Set default displays
    ui->lineEdit_SourceDir->setText(QString::fromStdString(args->At("source_directory").Value()));
    ui->lineEdit_AuthServer->setText(QString::fromStdString(args->At("auth_server").Value()));
    ui->lineEdit_EndServer->setText(QString::fromStdString(args->At("end_server").Value()));
    ui->lineEdit_Pattern->setText(QString::fromStdString(args->At("pattern").Value()));
    ui->lineEdit_Username->setText(QString::fromStdString(args->At("username").Value()));
    ui->lineEdit_Password->setText(QString::fromStdString(args->At("password").Value()));

    //Set up default tooltips
    ui->label_SourceDir->setToolTip(QString::fromStdString(args->At("source_directory").help));
    ui->label_AuthServer->setToolTip(QString::fromStdString(args->At("auth_server").help));
    ui->label_EndServer->setToolTip(QString::fromStdString(args->At("end_server").help));
    ui->label_Pattern->setToolTip(QString::fromStdString(args->At("pattern").help));
    ui->label_Username->setToolTip(QString::fromStdString(args->At("username").help));
    ui->label_Password->setToolTip(QString::fromStdString(args->At("password").help));
}

Dialog_UploadTool::~Dialog_UploadTool()
{
    delete ui;
}

void Dialog_UploadTool::AddToDisplayLog(QString val)
{
    ui->textEdit_Log->insertPlainText(QString(val + '\n'));
    LogFile::WriteToLog(val.toStdString());

    static QTextCursor c = ui->textEdit_Log->textCursor();
    c.movePosition(QTextCursor::End);
    ui->textEdit_Log->setTextCursor(c);
}

bool Dialog_UploadTool::ConfirmPattern(QString pattern)
{
    static QString compares[6] = {"%y", "%m", "%d", "%H", "%M", "%S"};
    bool result = true;

    for (int i = 0; i < 6; i++)
    {
        if (pattern.contains(compares[i]) == false)
        {
            AddToDisplayLog(QString("Pattern error: \"%1\" missing from pattern.").arg(compares[i]));
            result = false;
        }
    }

    return result;
}

void Dialog_UploadTool::on_checkBox_Recursive_stateChanged(int arg1)
{
    args->EditArg("recursive", arg1 == 0 ? "false" : "true");
}

void Dialog_UploadTool::on_lineEdit_AuthServer_textChanged(const QString &arg1)
{
    args->EditArg("auth_server", arg1.toStdString());
}

void Dialog_UploadTool::on_lineEdit_EndServer_textChanged(const QString &arg1)
{
    args->EditArg("end_server", arg1.toStdString());
}

void Dialog_UploadTool::on_lineEdit_SourceDir_textChanged(const QString &arg1)
{
    args->EditArg("source_directory", arg1.toStdString());
}

void Dialog_UploadTool::on_lineEdit_Pattern_textChanged(const QString &arg1)
{
    args->EditArg("pattern", arg1.toStdString());
}

void Dialog_UploadTool::on_lineEdit_Username_textEdited(const QString &arg1)
{
    args->EditArg("username", arg1.toStdString());
}

void Dialog_UploadTool::on_lineEdit_Password_textEdited(const QString &arg1)
{
    args->EditArg("password", arg1.toStdString());
}

bool Dialog_UploadTool::PassedDirectoryChecks()
{
    QString source_dir = QString::fromStdString(args->At("source_directory").Value());

    //Check if the source diretory exists
    if (source_dir == "")
    {
        AddToDisplayLog(QString("Source directory not set."));
        return false;
    }
    else if (!QDir(source_dir).exists())
    {
        AddToDisplayLog(QString("Source directory \"%1\" does not exist.").arg(source_dir));
        return false;
    }
    else
    {
        AddToDisplayLog(QString("Source directory \"%1\" found.").arg(source_dir));
    }

    QString pattern = QString::fromStdString(args->At("pattern").Value());

    if (pattern == "")
    {
        AddToDisplayLog(QString("Pattern not set."));
        return false;
    }
    else if (ConfirmPattern(pattern) == false)
    {
        AddToDisplayLog("Pattern check failed.");
        return false;
    }
    else
    {
        AddToDisplayLog("Pattern check passed.");
    }

    return true;
}

bool Dialog_UploadTool::PassedServerChecks()
{
    QString auth_server = QString::fromStdString(args->At("auth_server").Value());

    if (auth_server == "")
    {
        AddToDisplayLog(QString("Auth server not set."));
        return false;
    }

    return true;
}

void Dialog_UploadTool::on_pushButton_TestInput_clicked()
{
    if (m_ActiveUploadThread)
        return;

    if (!PassedDirectoryChecks())
        return;

    ui->textEdit_Log->clear();

    if (!m_ActiveUploadThread)
    {
        m_WorkerThread = new CurlUploadThread(this, true);
        m_ActiveUploadThread = true;
    }
    else
        return;

    connect(m_WorkerThread, &CurlUploadThread::finished, m_WorkerThread, &QObject::deleteLater);
    connect(m_WorkerThread, &CurlUploadThread::UpdateText, this, &Dialog_UploadTool::UpdateDisplayText);
    connect(m_WorkerThread, &CurlUploadThread::resultReady, this, &Dialog_UploadTool::HandleTestComplete);
    m_WorkerThread->start();
}

void Dialog_UploadTool::HandleUploadComplete()
{
    m_ActiveUploadThread = false;
    AddToDisplayLog("Upload process completed.");
}

void Dialog_UploadTool::HandleTestComplete()
{
    m_ActiveUploadThread = false;
    AddToDisplayLog("Test process completed.");
}

void Dialog_UploadTool::UpdateDisplayText(QString val)
{
    AddToDisplayLog(val);
}

void Dialog_UploadTool::on_pushButton_SelectSourceDir_clicked()
{
    QString source = QFileDialog::getExistingDirectory(this, ("Select Source Folder"), QString::fromStdString(args->At("source_directory").Value()));
    ui->lineEdit_SourceDir->setText(source);
}

void Dialog_UploadTool::closeEvent(QCloseEvent *event)
{
    hide();
    event->accept();
}

void Dialog_UploadTool::on_pushButton_Upload_clicked()
{
    if (m_ActiveUploadThread)
        return;

    if (!PassedDirectoryChecks() || !PassedServerChecks())
        return;

    if (!m_ActiveUploadThread)
    {
        m_WorkerThread = new CurlUploadThread(this, false);
        m_ActiveUploadThread = true;
    }
    else
        return;

    connect(m_WorkerThread, &CurlUploadThread::finished, m_WorkerThread, &QObject::deleteLater);
    connect(m_WorkerThread, &CurlUploadThread::UpdateText, this, &Dialog_UploadTool::UpdateDisplayText);
    connect(m_WorkerThread, &CurlUploadThread::resultReady, this, &Dialog_UploadTool::HandleTestComplete);
    m_WorkerThread->start();
}
