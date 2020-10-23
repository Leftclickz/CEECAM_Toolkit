#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "argumentmap.h"
#include "gcsdownloadthread.h"

#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QLineEdit>
#include <QStandardItemModel>

#include <aetl.h>

#include "google/cloud/storage/client.h"

#include <functional>
#include <stdio.h>
#include <time.h>
#include <locale.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("CEECAM Toolkit");

    //Create our argument map and parse our argument file
    args = new ArgumentMap();
    args->AddDefaultArguments();

    //Load patterns for download use.
    LoadPatterns();

    //Start the connection proxy
    {
        QString batchfile = QCoreApplication::applicationDirPath().append("/data/proxy/auto_connect.bat");
        QFile test(batchfile);

        if (test.exists())
        {
            proxyConnector = new QProcess(this);
            proxyConnector->start("\"" + batchfile + "\"", QStringList());

            proxySet = true;
        }
    }

    //Get the connection info from file
    if (proxySet)
    {
        //We only want a specific file so we'll filter out all other results.
        QStringList filters;
        filters << "connection.txt";

        //This is our data folder relative to the exe.
        QDir assetsFolder = QDir(QCoreApplication::applicationDirPath().append("/data/connection"));
        assetsFolder.setNameFilters(filters);
        assetsFolder.setFilter(QDir::Files);

        QFileInfoList files = assetsFolder.entryInfoList();
        std::string connInfo = "";

        //This should only ever run once but just in case.
        foreach (QFileInfo file, files)
        {
            QFile f(file.absoluteFilePath());
            if (!f.open(QFile::ReadOnly | QFile::Text)) continue;

            QTextStream in(&f);
            connInfo = in.readAll().toStdString();
            break;
        }

        if (connInfo != "")
            PGSQL::Connect(connInfo, CEEPLAYER_DB);

        conSet = true;
    }

    //Set up the GCS connection from json file.
    {
        //first we set the environment variable for google_application_credentials by searching for where the json file is since we need an absolute path.
        QStringList filters;
        filters << "*.json";

        //This is our data folder relative to the exe. It contains the json file.
        QDir assetsFolder = QDir(QCoreApplication::applicationDirPath().append("/data/connection"));
        assetsFolder.setNameFilters(filters);
        assetsFolder.setFilter(QDir::Files);

        QFileInfoList files = assetsFolder.entryInfoList();

        //This should only run once and with the json file we need. We set the environment var here then break out.
        foreach (QFileInfo file, files)
        {
            QString path = file.absoluteFilePath();
            qputenv("GOOGLE_APPLICATION_CREDENTIALS", path.toUtf8());
            gacSet = true;
            break;
        }

        if (gacSet)
        {
            //Create a client to interface with.
            namespace gcs = google::cloud::storage;
            clientHandle = gcs::Client::CreateDefaultClient();

            if (!clientHandle) {
                gacSet = false;
                return;
            }
        }
    }
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::AttemptGCSDownload(QString InputDir, QString OutputDir)
{
    if (!gacSet) return;

    //Ensure the base output dir exists and make it if possible.
    if (!QDir(OutputDir).exists())
    {
        if (!QDir().mkdir(OutputDir))
        {
            AddToDisplayLog(QString("Output directory does not exist and failed to be created. Argument: %1").arg(OutputDir));
            return;
        }

        AddToDisplayLog(QString("Output directory created. Argument: %1").arg(OutputDir));
    }

    time_t start_time_timestamp = args->GetAsTime("start_time");
    time_t end_time_timestamp = args->GetAsTime("end_time");
    time_t start_date_timestamp = args->GetAsDate("start_date");
    time_t end_date_timestamp = args->GetAsDate("end_date");

    std::string datetimeFormat = patterns[ui->comboBox_Pattern->currentText().toStdString()];

    bool daily_folder = args->At("no_daily_folder") && args->At("no_daily_folder").Value() == "false";
    std::string daily_folder_pattern = args->At("daily_folder_pattern").Value();

    time_t last_file_datetime = 0;
    int interval = args->At("interval") && args->At("interval").Value() != "" ? std::stoi(args->At("interval").Value().c_str()) : 0;

    auto list = clientHandle->ListObjects("ceeplayer", gcs::Prefix(InputDir.toStdString()));

    for (auto it = list.begin(); it != list.end(); ++it)
    {
        QString filepath = QString::fromStdString(it->value().name());
        QString filename = QDir(filepath).dirName();

        //If we're not dealing with an image it means it's a subdirectory.
        if (strncmp(it->value().content_type().c_str(), "image/jpeg", 5) != 0)
        {
            if (args->At("recursive").Value() == "true")
                AttemptGCSDownload(filepath, QString("%1/%2").arg(OutputDir, filename));
        }

        //If the pattern doesnt include the end filetype then only include the first 12 characters.
        std::string filename_to_match = filename.toStdString();
        if (datetimeFormat.find(".jpg") == std::string::npos)
            filename_to_match = filename_to_match.substr(0, 12);

        //Get a timestamp from the filename
        std::tm dt = {};
        ConvertStringToTM(dt, filename_to_match, datetimeFormat);
        dt.tm_isdst = 0;

        std::tm file_date_only = dt;
        file_date_only.tm_hour = 0;
        file_date_only.tm_min = 0;
        file_date_only.tm_sec = 0;

        std::tm file_time_only = dt;
        file_time_only.tm_year = 0;
        file_time_only.tm_yday = 0;
        file_time_only.tm_wday = 0;
        file_time_only.tm_mday = 0;
        file_time_only.tm_mon = 0;

        //mktime errors on times only, doing it manually
        time_t file_timestamp = (file_time_only.tm_hour * 3600) + (file_time_only.tm_min * 60) + file_time_only.tm_sec;
        time_t file_datestamp = std::mktime(&file_date_only);

        //If the file date is earlier than the start date then continue.
        if (file_datestamp < start_date_timestamp)
            continue;

        //If the file date is later than the end date then continue. Ignore if end_date wasn't set.
        if (end_date_timestamp != 0 && file_datestamp > end_date_timestamp)
            break;

        //If the file time is earlier than the start time then continue.
        if (file_timestamp < start_time_timestamp)
            continue;

        //If the file time is later than the end time then continue. Ignore if end_time wasn't set.
        if (end_time_timestamp != 0 && file_timestamp > end_time_timestamp)
            continue;

        if (interval > 0)
        {
            int diff = file_timestamp - last_file_datetime;

            if (diff < interval)
            {
                QString data = QString("skip:%1 last:%2 current:%3 seconds:%4").arg(filename
                                                                                    , QString::fromStdString(std::to_string(last_file_datetime))
                                                                                    , QString::fromStdString(std::to_string(file_timestamp))
                                                                                    , QString::fromStdString(std::to_string(file_timestamp - last_file_datetime))
                                                                                    );
                ui->textEdit_Log->append(data + '\n');
                ui->textEdit_Log->show();
                continue;
            }

            last_file_datetime = file_timestamp;
        }

        QString output_path;
        if (daily_folder)
        {
            char buffer[80];
            strftime(buffer, 80, daily_folder_pattern.c_str(), &dt);
            output_path = QString("%1/%2").arg(OutputDir, buffer);
        }
        else
        {
            output_path = OutputDir;
        }

        if (!QDir(output_path).exists())
        {
            if (!QDir().mkdir(output_path))
            {
                AddToDisplayLog(QString("Output directory does not exist and failed to be created. Argument: %1").arg(output_path));
                return;
            }

            AddToDisplayLog(QString("Output directory created. Argument: %1").arg(output_path));
        }

        output_path += "/" + filename;

        google::cloud::Status status = clientHandle->DownloadToFile("ceeplayer", filepath.toStdString(), output_path.toStdString());
        if (!status.ok()) throw std::runtime_error(status.message());

        AddToDisplayLog(QString("Copying: %1 to %2").arg(filepath, output_path));
    }
}

void MainWindow::LoadPatterns()
{
    //We only want a specific file so we'll filter out all other results.
    QStringList filters;
    filters << "patterns.txt";

    //This is our data folder relative to the exe.
    QDir assetsFolder = QDir(QCoreApplication::applicationDirPath().append("/data"));
    assetsFolder.setNameFilters(filters);
    assetsFolder.setFilter(QDir::Files);

    QFileInfoList files = assetsFolder.entryInfoList();
    std::string connInfo = "";

    //This should only ever run once but just in case.
    foreach (QFileInfo file, files)
    {
        QFile f(file.absoluteFilePath());
        if (!f.open(QFile::ReadOnly | QFile::Text)) continue;

        QTextStream in(&f);

        QString line;
        //filter every line and extract the argument and its variables
        while (in.readLineInto(&line))
        {
            std::string rawLine = line.toStdString();

            QString name = QString::fromStdString(rawLine.substr(0, rawLine.find("=")));
            rawLine = rawLine.substr(rawLine.find("=") + 2);
            QString val = QString::fromStdString(rawLine.substr(0, rawLine.find('\'')));

            patterns[name.toStdString()] = val.toStdString();
            ui->comboBox_Pattern->addItem(name);
        }
    }
}

void MainWindow::ConvertStringToTM(tm &timepoint, std::string data, std::string pattern)
{
    std::string copy = data;
    std::stringstream ss;

    //format std::string to be convertible into a time point
    {
        //delimit by colons
        ss << copy[0] << copy[1];
        for (size_t i = 2; i < copy.size(); i += 2)
            ss << ':' << copy[i] << copy[i + 1];
    }

    ss >> std::get_time(&timepoint, pattern.c_str());
}

void MainWindow::AddToDisplayLog(QString val)
{
    ui->textEdit_Log->append(val);
    LogFile::WriteToLog(val.toStdString());

    static QTextCursor c = ui->textEdit_Log->textCursor();
    c.movePosition(QTextCursor::End);
    ui->textEdit_Log->setTextCursor(c);
}


void MainWindow::on_pushButton_LoadTable_clicked()
{
    static bool initialized = false;

    if (!initialized){
        QStandardItemModel * model = new QStandardItemModel();
        model->setHorizontalHeaderItem(0, new QStandardItem(QString("NAME")));
        model->setHorizontalHeaderItem(1, new QStandardItem(QString("PID")));
        model->setHorizontalHeaderItem(2, new QStandardItem(QString("LID")));

        auto result = PGSQL::Query("SELECT id,project_id,name FROM public.cee_location WHERE deleted_at IS NULL ORDER BY id ASC;", CEEPLAYER_DB);

        model->setRowCount(result.size());

        for (pqxx::result::size_type i = 0; i != result.size(); i++)
        {
            std::string name = result[i]["name"].c_str();
            std::string pid = result[i]["project_id"].c_str();
            std::string lid = result[i]["id"].c_str();

            for (int x = 0; x < 3; x++)
            {
                switch (x)
                {
                case 0: model->setItem(i, x, new QStandardItem(QString::fromStdString(name))); break;
                case 1: model->setItem(i, x, new QStandardItem(QString::fromStdString(pid))); break;
                case 2: model->setItem(i, x, new QStandardItem(QString::fromStdString(lid))); break;
                default: break;
                }
            }
        }

        ui->tableView_DisplayPSQL->setModel(model);
        initialized = true;
    }
}


void MainWindow::on_pushButton_Download_clicked()
{
    static GCSDownloadThread *workerThread;

    if (!m_ActiveGCSDownloadThread)
    {
        workerThread = new GCSDownloadThread(this);
        m_ActiveGCSDownloadThread = true;
    }
    else
        return;

    connect(workerThread, &GCSDownloadThread::resultReady, this, &MainWindow::HandleGCSDownload);
    connect(workerThread, &GCSDownloadThread::finished, workerThread, &QObject::deleteLater);
    workerThread->start();
}

void MainWindow::on_lineEdit_StartDate_textChanged(const QString &arg1)
{
    args->EditArg("start_date", arg1.toStdString());
}

void MainWindow::on_lineEdit_EndDate_textChanged(const QString &arg1)
{
    args->EditArg("end_date", arg1.toStdString());
}

void MainWindow::on_lineEdit_InputDir_textChanged(const QString &arg1)
{
    args->EditArg("input_directory", "images/" + arg1.toStdString());
}

void MainWindow::on_lineEdit_OutputDir_textChanged(const QString &arg1)
{
    args->EditArg("output_directory", arg1.toStdString());
}

void MainWindow::on_lineEdit_StartTime_textChanged(const QString &arg1)
{
    args->EditArg("start_time", arg1.toStdString());
}

void MainWindow::on_lineEdit_EndTime_textChanged(const QString &arg1)
{
    args->EditArg("end_time", arg1.toStdString());
}

void MainWindow::on_checkBox_Recursive_stateChanged(int arg1)
{
    args->EditArg("recursive", arg1 == 0 ? "false" : "true");
}

void MainWindow::on_checkBox_DailyFolders_stateChanged(int arg1)
{
    args->EditArg("no_daily_folder", arg1 == 0 ? "true" : "false");
}

void MainWindow::HandleGCSDownload()
{
    m_ActiveGCSDownloadThread = false;
    AddToDisplayLog("Download process completed.");
}

