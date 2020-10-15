#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define CC_TK_VERSION "f50bd3f173113836cdcdf2486501e3a8905989eb"


#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_LoadTable_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
