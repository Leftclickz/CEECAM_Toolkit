#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtSql/QSqlError>
#include <QtSql/QSqlDatabase>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    //db.setHostName("127.0.0.1");
    //db.setDatabaseName("ceeplayer");
    //db.setUserName("postgres");
    //db.setPassword("do7Ny4q300xc518p");
    //bool ok = db.open();

}

MainWindow::~MainWindow()
{
    delete ui;
}

