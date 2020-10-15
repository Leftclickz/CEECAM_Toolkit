#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QStandardItemModel>

#include <aetl.h>

#include "google/cloud/storage/client.h"
#include <functional>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Get the connection info from file
    {
        //We only want a specific file so we'll filter out all other results.
        QStringList filters;
        filters << "connection.txt";

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
            connInfo = in.readAll().toStdString();
            break;
        }

        if (connInfo != "")
            PGSQL::Connect(connInfo, CEEPLAYER_DB);
    }

    //Set up the GCS connection from json file.
    {


        //first we set the environment variable for google_application_credentials by searching for where the json file is since we need an absolute path.
        QStringList filters;
        filters << "*.json";

        //This is our data folder relative to the exe. It contains the json file.
        QDir assetsFolder = QDir(QCoreApplication::applicationDirPath().append("/data"));
        assetsFolder.setNameFilters(filters);
        assetsFolder.setFilter(QDir::Files);

        //Flag to determine if we set the env correctly.
        static bool gacSet = false;

        QFileInfoList files = assetsFolder.entryInfoList();

        //This should only run once and with the json file we need. We set the environment var here then break out.
        foreach (QFileInfo file, files)
        {
            QString path = file.absoluteFilePath();
            qputenv("GOOGLE_APPLICATION_CREDENTIALS", path.toUtf8());
            gacSet = true;
            break;
        }

//        if (gacSet)
//        {
//            namespace gcs = google::cloud::storage;
//            google::cloud::StatusOr<gcs::Client> client = gcs::Client::CreateDefaultClient();
//            if (!client) {
//                return;
//              }

//            int count = 0;
//            gcs::ListBucketsReader bucket_list = client->ListBuckets();

//            for (auto&& bucket_metadata : bucket_list) {
//                  if (!bucket_metadata) {
//                    throw std::runtime_error(bucket_metadata.status().message());
//                  }

//                  std::cout << bucket_metadata->name() << "\n";
//                  ++count;
//            }



//            if (count == 0) {
//              std::cout << "No buckets in default project\n";
//            }



//        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
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

