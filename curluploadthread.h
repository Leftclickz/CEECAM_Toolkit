#ifndef CURLUPLOADTHREAD_H
#define CURLUPLOADTHREAD_H


//jpg defs
#define JPG_BOF_HEADER std::string("FFD8")
#define JPG_EOF_HEADER std::string("FFD9")

#include <QThread>
#include <QDir>
#include <QDirIterator>
#include <fstream>

#include "dialog_uploadtool.h"
#include "argumentmap.h"
#include "aetl.h"

class CurlUploadThread : public QThread
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
                    emit UpdateText(QString("Required value missing: %1").arg(QString::fromStdString(it->second.name)));
                    emit resultReady(this);
                    return;
                }
            }
        }

       CollectAllFilesForUpload();
       FilterFiles();

       emit resultReady(this);
    }

    void DisplayFilesToLog(bool displayUniqueFiles = true)
    {
        if (displayUniqueFiles)
        {
            //Display all files found in the log
            for (int i = 0; i < m_FilesToUpload.size(); i++)
            {
                emit UpdateText(QString("File found: %1").arg(m_FilesToUpload[i]));
            }
        }
        emit UpdateText(QString("%1 files found.").arg(m_FilesToUpload.size()));
    }

    std::string ToHex(const std::string& s, bool upper_case)
    {
        std::ostringstream ret;

        for (std::string::size_type i = 0; i < s.length(); ++i)
        {
            int z = s[i] & 0xff;
            ret << std::hex << std::setfill('0') << std::setw(2) << (upper_case ? std::uppercase : std::nouppercase) << z;
        }

        return ret.str();
    }


    //Simply collect all jpg/JPG files that exist in the source directory, we'll filter them later.
    void CollectAllFilesForUpload()
    {
        ArgumentMap* args = m_Owner->args;

        QString source = QString::fromStdString(args->At("source_directory").Value());
        QStringList files;

        if (args->At("recursive").Value() == "false")
        {
            QDirIterator it(source, QStringList() << "*.jpg" << "*.JPG", QDir::Files);
            while (it.hasNext())
            {
                files.push_back(it.next());
            }
        }
        else if (args->At("recursive").Value() == "true")
        {
            QDirIterator it(source, QStringList() << "*.jpg" << "*.JPG", QDir::Files, QDirIterator::Subdirectories);
            while (it.hasNext())
            {
                files.push_back(it.next());
            }
        }

        //Assign our member to remember them
        m_FilesToUpload = files;

        //Display all files found in the log
        emit UpdateText("Unfiltered file results:");
        DisplayFilesToLog(true);
    }

    //Now we filter based on our assigned pattern
    void FilterFiles()
    {
        std::string patternRaw = m_Owner->args->At("pattern").Value();
        std::string patternWithoutExtension;

        if (patternRaw.find('.') != std::string::npos)
            patternWithoutExtension = patternRaw.substr(0, patternRaw.find('.'));
        else
            patternWithoutExtension = patternRaw;

        //we need to isolate where inside the pattern that our timestamp exists. to do this we'll need to store a lot of indices for our REQUIRED pattern
        //we'll also keep track of the index latest down the line to compare with filenames to check if they're long enough to be considered.
        static std::string compares[6] = {"%y", "%m", "%d", "%H", "%M", "%S"};
        size_t timestampLocations[6] = {0};
        size_t minLengthRequiredByFilename = 0;
        for (int i = 0; i < 6; i++)
        {
            timestampLocations[i] = patternWithoutExtension.find(compares[i]);
            minLengthRequiredByFilename = timestampLocations[i] > minLengthRequiredByFilename ? timestampLocations[i] : minLengthRequiredByFilename;
        }
        minLengthRequiredByFilename += 2;//add 2 to push the index up and include the full length requirement

        QStringList filteredList;

        //Need to check now that these files contain a valid timestamp that corresponds with our pattern.
        for (int i = 0; i < m_FilesToUpload.size(); i++)
        {
            //If the file doesn't even have a length that matches the maximum requirement then we can ignore it.
            QString filename = QDir(m_FilesToUpload[i]).dirName();
            if (filename.length() < (int)minLengthRequiredByFilename)
            {
                emit UpdateText(QString("Filename %1 has an invalid timestamp.").arg(m_FilesToUpload[i]));
                continue;
            }

            QString timestamp;
            //Now we build a timestamp using the locations of the timestamp in the pattern.
            for (int j = 0; j < 6; j++)
            {
                timestamp += filename.mid((int)timestampLocations[j], 2) + ":";
            }
            timestamp = timestamp.left(timestamp.length() - 1);

            //Let's confirm that this datetime is valid.
            std::stringstream ss (timestamp.toStdString());
            std::tm dt;

            //Get the time in std::tm format. If this fails then continue since the supplied datetime is incorrect.
            ss >> std::get_time(&dt, "%y:%m:%d:%H:%M:%S");
            if (ss.fail())
            {
                emit UpdateText(QString("Filename %1 has an invalid timestamp.").arg(m_FilesToUpload[i]));
                continue;
            }

            //Now we actually create the seconds since epoch with the datetime. If this returns -1 this step failed which means
            //the supplied datetime is incorrect.
            std::time_t test = std::mktime(&dt);
            if (test == -1)
            {
                emit UpdateText(QString("Filename %1 has an invalid timestamp.").arg(m_FilesToUpload[i]));
                continue;
            }

            std::ifstream fileOpener;
            char headerofFile[2] = { 0 };
            std::string data;
            bool containsBOF = false, containsEOF = false;

            std::string fail = m_FilesToUpload[i].toStdString();

            fileOpener.open(fail, std::ios::in | std::ios::binary | std::ios::ate);

            //Get EOF tag
            {
                for (int i = 2; i > 0; i--)
                {
                    fileOpener.seekg(-i, std::ios::end);
                    fileOpener.get(headerofFile[2 - i]);
                }

                data = ToHex(headerofFile, true);
                data = data.substr(0, 4);

                containsEOF = (data == JPG_EOF_HEADER);
            }

            //Get BOF tag
            {
                fileOpener.seekg(0, std::ios::beg);
                fileOpener.read(headerofFile, sizeof(short));

                data = ToHex(headerofFile, true);
                data = data.substr(0, 4);

                containsBOF = (data == JPG_BOF_HEADER);
            }
            fileOpener.close();

            //if this file does not contain the appropriate end/begin metadata then it isn't a valid jpg file.
            if (containsBOF == false || containsEOF == false)
            {
                emit UpdateText(QString("Filename %1 is corrupted.").arg(m_FilesToUpload[i]));
                continue;
            }

            //If we got here we got a valid file.
            filteredList.push_back(m_FilesToUpload[i]);
        }

        //Update our file list
        m_FilesToUpload = filteredList;

        emit UpdateText("Filtered file results:");
        DisplayFilesToLog(true);
        emit UpdateText(QString("%1 files passed pattern filtration.").arg(m_FilesToUpload.length()));
    }


signals:
    void resultReady(CurlUploadThread* self);
    void UpdateText(QString text);

public:
    CurlUploadThread(class Dialog_UploadTool* owner, bool testRun) : m_Owner(owner), m_IsTestRun(testRun){}

private:

    Dialog_UploadTool* m_Owner = nullptr;
    bool m_IsTestRun = false;
    QStringList m_FilesToUpload;

};


#endif // CURLUPLOADTHREAD_H
