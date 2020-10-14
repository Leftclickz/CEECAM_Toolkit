#include "mainwindow.h"

#include <aetl.h>

#include <QApplication>

int main(int argc, char *argv[])
{

//    LogFile::BeginLogging();

//    pqxx::result result = PGSQL::Query("SELECT loc.id,loc.project_id,org.company_id FROM public.cee_location loc, public.cee_organization org WHERE loc.deleted_at IS NULL AND loc.project_id = org.id ORDER BY loc.id ASC;", CEEPLAYER_DB);

//    for (pqxx::result::size_type i = 0; i != result.size(); i++)
//    {
//        std::string projectName = result[i]["id"].c_str();
//        std::string projectInternalID = result[i]["project_id"].c_str();
//        std::string company_id = result[i]["company_id"].c_str();
//    }


    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("CEECAM");
    QCoreApplication::setApplicationName("CEECAM Toolkit");
    QCoreApplication::setApplicationVersion(CC_TK_VERSION);

    MainWindow w;
    w.show();
    return a.exec();
}
