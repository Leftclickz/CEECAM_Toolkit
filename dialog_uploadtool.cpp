#include "dialog_uploadtool.h"
#include "ui_dialog_uploadtool.h"

#include <QJsonObject>
#include <QJsonDocument>
#include <QTextCodec>

#include "argumentmap.h"
#include "curluploadthread.h"
#include "aetl.h"

#include <curl/curl.h>


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

QJsonObject ObjectFromString(const QString& in)
{
    QJsonObject obj;

    QJsonDocument doc = QJsonDocument::fromJson(in.toUtf8());

    // check validity of the document
    if(!doc.isNull())
    {
        if(doc.isObject())
        {
            obj = doc.object();
        }
        else
        {
            qDebug() << "Document is not an object" << "\n";
        }
    }
    else
    {
        qDebug() << "Invalid JSON...\n" << in << "\n";
    }

    return obj;
}

size_t writeFunc(void *ptr, size_t size, size_t nmemb, std::string* data)
{
    data->append((char*) ptr, size * nmemb);
    return size * nmemb;
}

void Dialog_UploadTool::on_pushButton_Upload_clicked()
{
    return;//remove this for testing purposes

    //Set codex to utf-8 until execution finishes here
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    auto curl = curl_easy_init();

    if (curl)
    {
        //Append content-type header
        struct curl_slist *hs=NULL;
        hs = curl_slist_append(hs, "Content-Type: application/json;charset=UTF-8");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, hs);

        //Set destination url to the auth server
        curl_easy_setopt(curl, CURLOPT_URL, args->At("auth_server").Value().c_str());

        //Set post fields
        QString postData = QString("{\"username\":\"%1\",\"password\":\"%2\"}").arg(
                    QString::fromStdString(args->At("username").Value()),
                    QString::fromStdString(args->At("password").Value())
                    );
        std::string rawData = postData.toStdString();
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, rawData.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, postData.length());

        //Setup callbacks
        std::string response_string, header_string;
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunc);
        curl_easy_setopt(curl, CURLOPT_HEADERDATA, &header_string);

        //Perform transaction
        auto res = curl_easy_perform(curl);

        //Get response info
        char* url;
        long response_code;
        double elapsed;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &elapsed);
        curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &url);

        //free the slist
        curl_slist_free_all(hs);

        //reset curl for later
        curl_easy_reset(curl);

        if (res != CURLE_OK)
        {
            //post relevant failure info to the message log
            AddToDisplayLog(QString::fromStdString(header_string));
            AddToDisplayLog(QString::fromStdString(response_string));
        }
        else
        {
            //Collect the token from the response
            QJsonObject tokenJson = ObjectFromString(QString::fromStdString(response_string));
            QString tokenID = tokenJson["token"].toString();

            //Display token used for this session
            AddToDisplayLog(QString("Token acquired from auth_server: %1").arg(tokenID));

            //Set destination url to the server endpoint
            curl_easy_setopt(curl, CURLOPT_URL, args->At("end_server").Value().c_str());

            //Set form for POST
            auto form = curl_mime_init(curl);

            //Add the file
            auto field = curl_mime_addpart(form);
            curl_mime_name(field, "sendfile");
            curl_mime_filedata(field, "D:/Work/test_two/200602011554-ibIgMg.jpg");

            /* Fill in the filename field */
            //field = curl_mime_addpart(form);
            //curl_mime_name(field, "filename");
            //curl_mime_data(field, "postit2.c", CURL_ZERO_TERMINATED);

            // Fill in the submit field too, even if this is rarely needed
            field = curl_mime_addpart(form);
            curl_mime_name(field, "submit");
            curl_mime_data(field, "send", CURL_ZERO_TERMINATED);

            //Add the token in the header
            QString tokenHeader = QString("Authorization: Token %1").arg(tokenID);
            std::string tokenHeaderCString = tokenHeader.toStdString();
            const char* tokenHeaderBytes = tokenHeaderCString.c_str();
            AddToDisplayLog(tokenHeaderBytes);
            curl_slist *hs=NULL;
            hs = curl_slist_append(hs, tokenHeaderBytes);
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, hs);

            //Add the form
            curl_easy_setopt(curl, CURLOPT_MIMEPOST, form);

            //Setup callbacks
            std::string response_string, header_string;
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunc);
            curl_easy_setopt(curl, CURLOPT_HEADERDATA, &header_string);

            //Perform transaction
            curl_easy_perform(curl);

            //Get response info
            char* url;
            long response_code;
            double elapsed;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
            curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &elapsed);
            curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &url);

            //post relevant info to the message log
            AddToDisplayLog(QString::fromStdString(header_string));
            AddToDisplayLog(QString::fromStdString(response_string));

            //free the slist
            curl_slist_free_all(hs);
        }

        //cleanup
        curl_easy_cleanup(curl);
    }

    QTextCodec::setCodecForLocale(nullptr);
}
