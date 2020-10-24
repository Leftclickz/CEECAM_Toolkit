#ifndef GCSDOWNLOADTHREAD_H
#define GCSDOWNLOADTHREAD_H

#include <QThread>

#include "mainwindow.h"
#include "argumentmap.h"
#include "LogFile.h"

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
                {
                    emit resultReady(this);
                    return;
                }
            }
        }

        QString input_dir = QString::fromStdString(m_Owner->args->At("input_directory").Value());
        QString output_dir = QString::fromStdString(m_Owner->args->At("output_directory").Value());
        AttemptGCSDownload(input_dir, output_dir);

        emit resultReady(this);
    }

    void AttemptGCSDownload(QString InputDir, QString OutputDir)
    {
           if (!m_Owner->gacSet) return;

           //Ensure the base output dir exists and make it if possible.
           if (!QDir(OutputDir).exists())
           {
               if (!QDir().mkdir(OutputDir))
               {
                   emit UpdateText(QString("Output directory does not exist and failed to be created. Argument: %1").arg(OutputDir));
                   return;
               }

               emit UpdateText(QString("Output directory created. Argument: %1").arg(OutputDir));
           }

           std::tm start_date_tm, end_date_tm;
           std::string start = m_Owner->args->At("start_date").Value(), end =m_Owner->args->At("end_date").Value();
           _FindAndReplaceAll(start, "/", "");
           _FindAndReplaceAll(end, "/", "");
           m_Owner->ConvertStringToTM(start_date_tm, start, "%y:%m:%d");
           m_Owner->ConvertStringToTM(end_date_tm, end, "%y:%m:%d");

           time_t start_date_timestamp = std::mktime(&start_date_tm);
           time_t end_date_timestamp = std::mktime(&end_date_tm);
           time_t start_time_timestamp = m_Owner->args->GetAsTime("start_time");
           time_t end_time_timestamp = m_Owner->args->GetAsTime("end_time");

           std::string datetimeFormat = m_Owner->patterns[m_Owner->GetPatternText().toStdString()];

           bool daily_folder = m_Owner->args->At("no_daily_folder") && m_Owner->args->At("no_daily_folder").Value() == "false";
           std::string daily_folder_pattern = m_Owner->args->At("daily_folder_pattern").Value();

           time_t last_file_datetime = 0;
           int interval = m_Owner->args->At("interval") && m_Owner->args->At("interval").Value() != "" ? std::stoi(m_Owner->args->At("interval").Value().c_str()) : 0;

           auto list = m_Owner->clientHandle->ListObjects("ceeplayer", gcs::Prefix(InputDir.toStdString()));
           std::stringstream stream, streamtwo;

           stream << start_time_timestamp;
           streamtwo << end_time_timestamp;
           QString display = QString("Start timestamp: %1\nEnd timestamp: %2").arg(QString::fromStdString(stream.str()), QString::fromStdString(streamtwo.str()));
           LogFile::WriteToLog(display.toStdString());
           stream.str(std::string());
           streamtwo.str(std::string());

           stream << start_date_timestamp;
           streamtwo << end_date_timestamp;
           display = QString("Start datestamp: %1\nEnd datestamp: %2").arg(QString::fromStdString(stream.str()), QString::fromStdString(streamtwo.str()));
           LogFile::WriteToLog(display.toStdString());
           stream.str(std::string());
           streamtwo.str(std::string());

           char buffer[30], buffertwo[30];
           strftime(buffer, 30, "%y:%m:%d:%H:%M:%S", &start_date_tm);
           strftime(buffertwo, 30, "%y:%m:%d:%H:%M:%S", &end_date_tm);
           std::string test(buffer), testtwo(buffertwo);
           LogFile::WriteToLog(test + " : " + testtwo);

           for (auto it = list.begin(); it != list.end(); ++it)
           {
               QString filepath = QString::fromStdString(it->value().name());
               QString filename = QDir(filepath).dirName();

               //If we're not dealing with an image it means it's a subdirectory.
               if (strncmp(it->value().content_type().c_str(), "image/jpeg", 5) != 0)
               {
                   if (m_Owner->args->At("recursive").Value() == "true")
                       AttemptGCSDownload(filepath, QString("%1/%2").arg(OutputDir, filename));
               }

               //If the pattern doesnt include the end filetype then only include the first 12 characters.
               std::string filename_to_match = filename.toStdString();
               if (datetimeFormat.find(".jpg") == std::string::npos)
                   filename_to_match = filename_to_match.substr(0, 12);

               //Get a timestamp from the filename
               std::tm dt = {};
               m_Owner->ConvertStringToTM(dt, filename_to_match, datetimeFormat);
               dt.tm_isdst = 0;

               char buffer[30];
               strftime(buffer, 30, "%y:%m:%d:%H:%M:%S", &dt);
               std::string test(buffer);
               LogFile::WriteToLog(test);

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

               stream << file_datestamp;
               QString display = QString("File datestamp: %1").arg(QString::fromStdString(stream.str()));
               LogFile::WriteToLog(display.toStdString());
               stream.str(std::string());

               stream << file_timestamp;
               display = QString("File timestamp: %1").arg(QString::fromStdString(stream.str()));
               LogFile::WriteToLog(display.toStdString());
               stream.str(std::string());


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
                       emit UpdateText(data + '\n');
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
                       emit UpdateText(QString("Output directory does not exist and failed to be created. Argument: %1").arg(output_path));
                       return;
                   }

                   emit UpdateText(QString("Output directory created. Argument: %1").arg(output_path));
               }

               output_path += "/" + filename;

               google::cloud::Status status = m_Owner->clientHandle->DownloadToFile("ceeplayer", filepath.toStdString(), output_path.toStdString());
               if (!status.ok()) throw std::runtime_error(status.message());

               emit UpdateText(QString("Copying: %1 to %2").arg(filepath, output_path));
           }
    }

signals:
    void resultReady(GCSDownloadThread* self);
    void UpdateText(QString text);

public:
    GCSDownloadThread(class MainWindow* owner) : m_Owner(owner){}

private:

    MainWindow* m_Owner = nullptr;
};

#endif // GCSDOWNLOADTHREAD_H
