#include "dialog_uploadtool.h"
#include "ui_dialog_uploadtool.h"

#include "argumentmap.h"
#include "curluploadthread.h"
#include "aetl.h"

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

void Dialog_UploadTool::on_pushButton_TestInput_clicked()
{
    ui->textEdit_Log->clear();

    bool canContinue = true;

    QString source_dir = QString::fromStdString(args->At("source_directory").Value());

    //Check if the source diretory exists
    if (source_dir == "")
    {
        AddToDisplayLog(QString("Source directory not set."));
        canContinue = false;
    }
    else if (!QDir(source_dir).exists())
    {
        AddToDisplayLog(QString("Source directory \"%1\" does not exist.").arg(source_dir));
        canContinue = false;
    }
    else
    {
        AddToDisplayLog(QString("Source directory \"%1\" found.").arg(source_dir));
    }

    QString pattern = QString::fromStdString(args->At("pattern").Value());

    if (pattern == "")
    {
        AddToDisplayLog(QString("Pattern not set."));
        canContinue = false;
    }
    else if (ConfirmPattern(pattern) == false)
    {
        AddToDisplayLog("Pattern check failed.");
        canContinue = false;
    }
    else
    {
        AddToDisplayLog("Pattern check passed.");
    }

    if (canContinue == false)
    {
        return;
    }

    static CurlUploadThread *workerThread;

    if (!m_ActiveUploadThread)
    {
        workerThread = new CurlUploadThread(this, true);
        m_ActiveUploadThread = true;
    }
    else
        return;


    connect(workerThread, &CurlUploadThread::finished, workerThread, &QObject::deleteLater);
    connect(workerThread, &CurlUploadThread::UpdateText, this, &Dialog_UploadTool::UpdateDisplayText);
    connect(workerThread, &CurlUploadThread::resultReady, this, &Dialog_UploadTool::HandleTestComplete);
    workerThread->start();
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


