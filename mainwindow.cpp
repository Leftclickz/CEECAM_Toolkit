#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "argumentmap.h"

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

    args = new ArgumentMap();
    args->AddDefaultArguments();

    LoadPatterns();

    //TODO: the proxy needs to be initialized before I can actually run this connection. ADD THIS.
    //parse our general argument list and add them to the map.
    //Get the connection info from file
//    {
//        //We only want a specific file so we'll filter out all other results.
//        QStringList filters;
//        filters << "connection.txt";

//        //This is our data folder relative to the exe.
//        QDir assetsFolder = QDir(QCoreApplication::applicationDirPath().append("/data"));
//        assetsFolder.setNameFilters(filters);
//        assetsFolder.setFilter(QDir::Files);

//        QFileInfoList files = assetsFolder.entryInfoList();
//        std::string connInfo = "";

//        //This should only ever run once but just in case.
//        foreach (QFileInfo file, files)
//        {
//            QFile f(file.absoluteFilePath());
//            if (!f.open(QFile::ReadOnly | QFile::Text)) continue;

//            QTextStream in(&f);
//            connInfo = in.readAll().toStdString();
//            break;
//        }

//        if (connInfo != "")
//            PGSQL::Connect(connInfo, CEEPLAYER_DB);
//    }

    //Set up the GCS connection from json file.
    {
        //first we set the environment variable for google_application_credentials by searching for where the json file is since we need an absolute path.
        QStringList filters;
        filters << "*.json";

        //This is our data folder relative to the exe. It contains the json file.
        QDir assetsFolder = QDir(QCoreApplication::applicationDirPath().append("/data"));
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

            //AttemptGCSDownload("images/1/1", "D:/Work/test");

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

    auto list = clientHandle->ListObjects("ceeplayer", gcs::Prefix(InputDir.toStdString()));

    time_t start_time_timestamp = args->GetAsTime("start_time", "%H:%M:%S");
    time_t end_time_timestamp = args->GetAsTime("end_time", "%H:%M:%S");
    time_t start_date_timestamp = args->GetAsTime("start_date", "%y/%m/%d");
    time_t end_date_timestamp = args->GetAsTime("end_date", "%y/%m/%d");

    std::string datetimeFormat = patterns[ui->comboBox_Pattern->currentText().toStdString()];
    std::string daily_folder_pattern = args->At("daily_folder_pattern").Value();

    time_t last_file_datetime = 0;


    int interval = 0;

    //grab our interval
    if (args->At("interval") && args->At("interval").Value() != "")
    {
        interval = std::stoi(args->At("interval").Value().c_str());
    }

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
        file_date_only.tm_isdst = 0;

        std::tm file_time_only = dt;
        file_time_only.tm_year = 0;
        file_time_only.tm_yday = 0;
        file_time_only.tm_wday = 0;
        file_time_only.tm_mday = 0;
        file_time_only.tm_mon = 0;

        time_t file_timestamp = std::mktime(&file_time_only);
        time_t file_datestamp = std::mktime(&file_date_only);


        if (args->At("start_date") && args->At("start_date").Value() != "")
        {
            if (file_datestamp < start_date_timestamp)
                continue;
        }

        if (args->At("end_date") && args->At("end_date").Value() != "")
        {
            if (file_datestamp > end_date_timestamp)
                continue;
        }

        if (args->At("start_time") && args->At("start_time").Value() != "")
        {
            if (file_timestamp < start_time_timestamp)
                continue;
        }

        if (args->At("end_time") && args->At("end_time").Value() != "")
        {
            if (file_timestamp > end_time_timestamp)
                continue;
        }

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
                ui->textEdit_Log->insertPlainText(data);
                continue;
            }

            last_file_datetime = file_timestamp;
        }

        QString output_path;
        if (args->At("no_daily_folder") && args->At("no_daily_folder").Value() == "false")
        {
            char buffer[80];
            strftime(buffer, 80, daily_folder_pattern.c_str(), &dt);
            QString daily_folder = QString("%1/%2").arg(OutputDir, "");
        }
        else
        {
            output_path = QString("%1/%2").arg(OutputDir, filename);
        }


        if (!QDir(OutputDir).exists())
            QDir().mkdir(OutputDir);

        google::cloud::Status status = clientHandle->DownloadToFile("ceeplayer", filepath.toStdString(), output_path.toStdString());
        if (!status.ok()) throw std::runtime_error(status.message());
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
    //confirm all needed values are set
    for (auto it = args->begin(); it != args->end(); ++it)
    {
        if (it->second.required == true)
        {
            if (it->second.Value() == "")
                return;
        }
    }

    ui->textEdit_Log->insertPlainText("Beginning download.");
    AttemptGCSDownload(QString::fromStdString(args->At("input_directory").Value()),QString::fromStdString(args->At("output_directory").Value()));
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
