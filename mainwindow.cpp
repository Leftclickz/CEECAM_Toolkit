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
    delete args;
    delete proxyConnector;
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
    ui->textEdit_Log->insertPlainText(QString(val + '\n'));
    LogFile::WriteToLog(val.toStdString());

    static QTextCursor c = ui->textEdit_Log->textCursor();
    c.movePosition(QTextCursor::End);
    ui->textEdit_Log->setTextCursor(c);
}

QString MainWindow::GetPatternText()
{
    return ui->comboBox_Pattern->currentText();
}

void MainWindow::ShutdownProxy()
{
    //Need the find the exe name
    QStringList filters;
    filters << "*.exe";

    //This is our proxy folder
    QDir assetsFolder = QDir(QCoreApplication::applicationDirPath().append("/data/proxy"));
    assetsFolder.setNameFilters(filters);
    assetsFolder.setFilter(QDir::Files);

    QFileInfoList files = assetsFolder.entryInfoList();

    bool found = false;

    QStringList args("/im");

    //This should only ever run once but just in case.
    foreach (QFileInfo file, files)
    {
        args.push_back(file.fileName());
        found = true;
        break;
    }

    //Assuming we found our exe we're going to taskkill it.
    if (found)
    {
        args.push_back("/f");

        QProcess p;
        p.start("taskkill", args);
        p.waitForFinished();
    }
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

    AddToDisplayLog(QString("Download process begin. Input directory: %1")
                    .arg(QString::fromStdString(args->At("input_directory").Value())));

    connect(workerThread, &GCSDownloadThread::finished, workerThread, &QObject::deleteLater);
    connect(workerThread, &GCSDownloadThread::resultReady, this, &MainWindow::HandleGCSDownload);
    connect(workerThread, &GCSDownloadThread::UpdateText, this, &MainWindow::HandleGCSUpdateText);
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

void MainWindow::HandleGCSUpdateText(QString text)
{
    AddToDisplayLog(text);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (proxySet)
    {
        proxyConnector->kill();
        ShutdownProxy();
    }
    if (conSet)
    {
        PGSQL::DisconnectAll();
    }

    event->accept();
}

