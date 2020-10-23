#ifndef ARGUMENTMAP_H
#define ARGUMENTMAP_H

#include <string>
#include <sstream>
#include <map>

#include <QDir>
#include <QStringList>
#include <QCoreApplication>
#include <QTextStream>
#include <ctime>
#include <iomanip>

struct Argument
{
    Argument()
    {
        arg = "";
        name = "";
        defaultVal = "";
        val = "";
        help = "";
    }
    Argument(std::string Arg, std::string Name, std::string DefaultVal, std::string Val, std::string Help, bool Required)
    {
        arg = Arg;
        name = Name;
        defaultVal = DefaultVal;
        val = Val;
        help = Help;
        required = Required;
    }

    inline std::string Value()
    {
        if (val != "")
            return val;
        else
            return defaultVal;
    }

    std::string arg, name, defaultVal, val, help;
    bool required = false;

    explicit operator bool()
    {
        if (required == false)
            return true;

        if (val == "" && defaultVal == "")
            return false;

        return true;
    }
};

class ArgumentMap
{
public:

    inline void AddArg(std::string arg, std::string name, std::string defaultVal, std::string val, std::string help, bool required)
    {
        if (args.count(name) == 1)
        {
            return;
        }

        args[name] = Argument(arg,name,defaultVal,val,help,required);
    }

    inline void EditArg(std::string name, std::string val)
    {
        if (args.count(name) == 0)
        {
            return;
        }

        args[name].val = val;
    }

    inline void AddDefaultArguments()
    {
        //We only want a specific file so we'll filter out all other results.
        QStringList filters;
        filters << "default_args.txt";

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
                std::string iteratedLine = rawLine;

                std::string arg, name, defaultVal, help;//our argument vals
                bool required = false;

                arg = iteratedLine.substr(0,iteratedLine.find(','));

                iteratedLine = iteratedLine.substr(iteratedLine.find(',') + 2);
                iteratedLine = iteratedLine.substr(iteratedLine.find("name") + 6);
                name  = iteratedLine.substr(0, iteratedLine.find("\""));

                iteratedLine = iteratedLine.substr(iteratedLine.find('\"') + 3);

                if (iteratedLine.find("default") != std::string::npos)
                {
                    iteratedLine = iteratedLine.substr(iteratedLine.find("default") + 9);
                    defaultVal  = iteratedLine.substr(0, iteratedLine.find('\"'));
                    iteratedLine = iteratedLine.substr(iteratedLine.find('\"') + 3);
                }



                iteratedLine = iteratedLine.substr(iteratedLine.find("help") + 6);
                help  = iteratedLine.substr(0, iteratedLine.find("\""));

                if (iteratedLine.find("required") != std::string::npos)
                {
                    required = iteratedLine.substr(iteratedLine.find("required") + 9) == "true";
                }

                AddArg(arg, name, defaultVal, "", help, required);
                continue;
            }
        }
    }

    inline Argument At(std::string val)
    {
        if (args.count(val) == 0)
        {
            return Argument();
        }

        return args[val];
    }

    std::_Tree_iterator<std::_Tree_val<std::_Tree_simple_types<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, Argument>>>> begin()
    {
        return args.begin();
    }

    std::_Tree_iterator<std::_Tree_val<std::_Tree_simple_types<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, Argument>>>> end()
    {
        return args.end();
    }

    time_t GetAsDate(std::string name)
    {
        if (At(name))
        {
            if (At(name).Value() != "")
            {
                std::string dateFormat("%y/%m/%d");
                std::string contents = At(name).Value();
                std::stringstream ss { contents };
                std::tm dt;
                ss >> std::get_time(&dt, dateFormat.c_str());
                return std::mktime(&dt);
            }
        }

        return time_t();
    }

    time_t GetAsTime(std::string name)
    {
        if (At(name))
        {
            if (At(name).Value() != "")
            {
                std::string dateFormat("%H:%M:%S");
                std::string contents = At(name).Value();
                std::stringstream ss { contents };
                std::tm dt;
                ss >> std::get_time(&dt, dateFormat.c_str());
                return (dt.tm_hour * 3600) + (dt.tm_min * 60) + dt.tm_sec;
            }
        }

        return time_t();
    }


private:
    std::map<std::string, Argument> args;
};

#endif // ARGUMENTMAP_H
