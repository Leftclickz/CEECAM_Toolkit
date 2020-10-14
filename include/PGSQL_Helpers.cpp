#include "aetl.h"

#include <iostream>
#include <windows.h>

#include <Qstring>

std::map<std::string, DatabaseConnection> PGSQL::ConnectionsMap;
PGSQL::VirtualMachinesData PGSQL::UpdateInformation;
bool PGSQL::IsRegisteringToAETLDB = true;

void PGSQL::Connect(std::string ConnectionInfo, std::string ConnectionName)
{
	PGSQL_RETURN_IF_EXISTS(ConnectionName,);

	ConnectionsMap[ConnectionName].SetConnection(ConnectionInfo);
}

void PGSQL::Disconnect(std::string ConnectionName)
{
	PGSQL_RETURN_IF_NOEXIST(ConnectionName,);
	ConnectionsMap[ConnectionName].Disconnect();
}

void PGSQL::DisconnectAll()
{
	for (auto const& x : ConnectionsMap)
	{
		Disconnect(x.first);
	}

	ConnectionsMap.clear();
}

bool PGSQL::ConnectionExists(std::string ConnectionName)
{
	if (ConnectionsMap.find(ConnectionName) == ConnectionsMap.end()) 
	{
		return false;
	}
	else 
	{
		return true;
	}
}

pqxx::result PGSQL::Query(std::string sqlQuery, std::string ConnectionName, bool DisplayQuery)
{
	pqxx::result res;

	PGSQL_RETURN_IF_NOEXIST(ConnectionName, res);

	try 
	{
		res = ConnectionsMap[ConnectionName].Query(sqlQuery, DisplayQuery);
	}
	catch (const std::exception & e)
	{
		//error reason
		std::cerr << e.what() << std::endl;
	}


	//std::this_thread::sleep_for(std::chrono::milliseconds(100));
	return res;
}

void PGSQL::SendUpdateToPGSQL()
{
	if (IsRegisteringToAETLDB)
	{
		//Ensure we're registered still.
		RegisterToAETLDB(false);

		std::string pSQL = std::string("UPDATE public." + DATABASE_VM_LOG + " SET ") +
			PROJECT_FIELD_UPDATEDAT + " ='" + _CurrentDateTime() + "', " +
			PROJECT_FIELD_ACTIVITY + "='" + PGSQL::UpdateInformation.activity + "', " +
			PROJECT_FIELD_LASTMSG + "='" + PGSQL::UpdateInformation.lastmessage + "' " +
			"WHERE " + PROJECT_FIELD_HOSTNAME + "='" + PGSQL::UpdateInformation.host + "'";

		(void)PGSQL::Query(pSQL, AETL_DB);
	}
}

void PGSQL::RegisterToAETLDB(bool FirstRun)
{
	if (IsRegisteringToAETLDB)
	{
		if (FirstRun == true)
			PGSQL::UpdateInformation.SetInfo("Online", "Initializing.");

		std::string pSQL = "INSERT INTO public." + DATABASE_VM_LOG + " (" + PROJECT_FIELD_HOSTNAME + ", " + PROJECT_FIELD_UPDATEDAT + ", " + PROJECT_FIELD_ACTIVITY + ", " + PROJECT_FIELD_LASTMSG + ") SELECT '"
			+ PGSQL::UpdateInformation.host + "', '"
			+ _CurrentDateTime() + "', '"
			+ PGSQL::UpdateInformation.activity + "', '"
			+ PGSQL::UpdateInformation.lastmessage + "'"
			+ " WHERE NOT EXISTS (SELECT 1 FROM public." + DATABASE_VM_LOG + " WHERE " + PROJECT_FIELD_HOSTNAME + "='" + PGSQL::UpdateInformation.host + "');";

		auto ret = PGSQL::Query(pSQL, AETL_DB);
	}
}

void PGSQL::LogMessageToAETLDB(std::string Severity, std::string Message)
{
	if (IsRegisteringToAETLDB)
	{
		std::string pSQL = "INSERT INTO public." + DATABASE_EVENT_LOG + " (" + PROJECT_FIELD_HOSTNAME + ", " + PROJECT_FIELD_SVRTY + ", " + PROJECT_FIELD_MSG + ") "
			+ "SELECT '" + PGSQL::UpdateInformation.host + "', '" + Severity + "', '" + Message + "';";

		(void)PGSQL::Query(pSQL, AETL_DB);
	}
}

const std::string PGSQL::_CurrentDateTime()
{
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	auto ret = localtime_s(&tstruct, &now);
	strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);

	std::string data(buf);

	return data;
}

void PGSQL::_FindAndReplaceAll(std::string& data, std::string toSearch, std::string replaceStr)
{
	// Get the first occurrence
	size_t pos = data.find(toSearch);

	// Repeat till end is reached
	while (pos != std::string::npos)
	{
		// Replace this occurrence of Sub std::string
		data.replace(pos, toSearch.size(), replaceStr);
		// Get the next occurrence from the current position
		pos = data.find(toSearch, pos + replaceStr.size());
	}
}


PGSQL::VirtualMachinesData::VirtualMachinesData()
{
	//Determine host name
	if (host == "")
	{
		TCHAR* infoBuf = new TCHAR[32767];
		DWORD  bufCharCount = 32767;

		// Get and display the name of the computer.
		if (!GetComputerName(infoBuf, &bufCharCount))
		{
			std::cout << "Cannot find computer name" << std::endl;
			LogFile::WriteToLog("Computer name not found.");
			host = "UNKNOWN HOST";
			delete[] infoBuf;
			return;
		}
        QString str = QString::fromWCharArray(infoBuf);
        str.toStdString();
        host = str.toStdString();
		delete[] infoBuf;
	}
}
