#pragma once
#include <string>
#include <vector>
#include <map>

//libpq
#include <../submodules/libpqxx/include/pqxx/pqxx>

//Tables
#ifndef DATABASE_WHITELIST
#define DATABASE_WHITELIST std::string("Whitelist")
#endif

//Tables
#define DATABASE_IMAGE_DIRECTORIES "ImageDirectories"
#define DATABASE_BLACKLIST "Blacklist"
#define DATABASE_PROJECT_LOG "ProjectBuildLog"
#define DATABASE_VM_LOG std::string("\"VirtualMachines\"")
#define DATABASE_EVENT_LOG std::string("\"EventLog\"")


//ProjectBuildLog table defines
#define PROJECT_FIELD_PROJECTID "\"ProjectID\""
#define PROJECT_FIELD_LOCATIONID "\"LocationID\""
#define PROJECT_FIELD_CREATEDAT "\"CreatedAt\""
#define PROJECT_FIELD_UPDATEDAT std::string("\"UpdatedAt\"")
#define PROJECT_FIELD_NAME "\"Name\""
#define PROJECT_FIELD_IMAGETYPE "\"ImageType\""
#define PROJECT_FIELD_PROJECTBUILT "\"ProjectBuilt\""
#define PROJECT_FIELD_VIDEORENDERED "\"VideoRendered\""
#define PROJECT_FIELD_UPLOADED "\"Uploaded\""
#define PROJECT_FIELD_LASTDL "\"LastDownload\""
#define PROJECT_FIELD_STATUS "\"Status\""
#define PROJECT_FIELD_RETRIES "\"Retries\""

//VirtualMachines table defines
#define PROJECT_FIELD_HOSTNAME std::string("\"HostName\"")
#define PROJECT_FIELD_ACTIVITY std::string("\"Activity\"")
#define PROJECT_FIELD_LASTMSG std::string("\"LastMessage\"")

//EventLog table defines
#define PROJECT_FIELD_MSG std::string("\"Message\"")
#define PROJECT_FIELD_SVRTY std::string("\"Severity\"")

#define PGSQL_RETURN_IF_EXISTS(_ID_, _TO_RET) if (ConnectionExists(_ID_) == true) return _TO_RET;
#define PGSQL_RETURN_IF_NOEXIST(_ID_, _TO_RET) if (ConnectionExists(_ID_) == false) return _TO_RET;
#define PGSQL_CONTINUE_IF_EXISTS(_ID_, _TO_RET) PGSQL_RETURN_IF_NOEXIST(_ID_, _TO_RET)
#define PGSQL_CONTINUE_IF_NOEXIST(_ID_, _TO_RET) PGSQL_RETURN_IF_EXISTS (_ID_, _TO_RET)

class DatabaseConnection;

namespace PGSQL 
{
	//Storage for holding a payload to send to the VirtualMachines table
	struct VirtualMachinesData
	{
		VirtualMachinesData();

		std::string host = "";
		std::string activity = "";
		std::string lastmessage = "";

		void SetInfo(std::string Activity = "", std::string Message = "")
		{
			if (Activity != "")
				activity = Activity;
			if (Message != "")
				lastmessage = Message;
		}
	};
	
	//Storage struct for holding a payload to send to the EventLog table
	struct EventLogData
	{
	public:
		EventLogData(std::string HOSTNAME, std::string STATUS, std::string LASTMSG) : hostname(HOSTNAME), status(STATUS), message(LASTMSG) {}
		std::string hostname;
		std::string status;
		std::string message;
	};

	void Connect(std::string ConnectionInfo, std::string ConnectionName);//Connect a database
	void Disconnect(std::string ConnectionName);//Disconnect a database
	void DisconnectAll();//Disconnect all databases

	bool ConnectionExists(std::string ConnectionName);//Check if connection has been established

	pqxx::result Query(std::string sqlQuery, std::string ConnectionName, bool DisplayQuery = false);//Raw query on a connection without any prior formatting.

	//Register this host to AETL DB's virtual machine and event log tables
	void RegisterToAETLDB(bool FirstRun = true);

	//Send update to AETL DB's virtual machines table
	void SendUpdateToPGSQL();

	//Send a message to AETL DB's event log
	void LogMessageToAETLDB(std::string Severity, std::string Message);

	extern VirtualMachinesData UpdateInformation;//the current update payload to AETL DB
	extern bool IsRegisteringToAETLDB;//whether or not we're sending updates to AETL DB

	extern std::map<std::string, DatabaseConnection> ConnectionsMap;//our list of connections to databases

	const std::string _CurrentDateTime();
	void _FindAndReplaceAll(std::string& data, std::string toSearch, std::string replaceStr);

};