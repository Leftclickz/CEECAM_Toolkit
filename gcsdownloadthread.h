#ifndef GCSDOWNLOADTHREAD_H
#define GCSDOWNLOADTHREAD_H

#include <QThread>

#include "mainwindow.h"
#include "argumentmap.h"

class GCSDownloadThread : public QThread
{
    Q_OBJECT
    void run() override
    {
        //Safety check.
        if (m_Owner == nullptr) return;

        //confirm all needed values are set
        for (auto it = m_Owner->args->begin(); it != m_Owner->args->end(); ++it)
        {
            if (it->second.required == true)
            {
                if (it->second.Value() == "")
                    return;
            }
        }

        m_Owner->AddToDisplayLog(QString("Download process begin. Input directory: %1")
                                 .arg(QString::fromStdString(m_Owner->args->At("input_directory").Value())));

        m_Owner->AttemptGCSDownload(
                    QString::fromStdString(m_Owner->args->At("input_directory").Value())
                    ,QString::fromStdString(m_Owner->args->At("output_directory").Value()));

        emit resultReady(this);
    }

signals:
    void resultReady(GCSDownloadThread* self);

public:
    GCSDownloadThread(class MainWindow* owner) : m_Owner(owner){}

private:

    MainWindow* m_Owner = nullptr;
};

#endif // GCSDOWNLOADTHREAD_H
