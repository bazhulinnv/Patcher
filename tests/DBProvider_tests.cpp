#include <DBProvider/DBProvider.h>
#include "../include/DBProvider/DBProviderLogger.h"
#include <iostream>
#include <pqxx/pqxx>

bool testStandardConnectionMethod()
{
	using namespace pqxx;

	try
	{
		connection conn("dbname = Doors user = doo password = rc hostaddr = 127.0.0.1 port = 5432");

		if (conn.is_open())
		{
			std::cout << "Opened database successfully: " << conn.dbname() << std::endl;
			return true;
		}
		
		std::cout << "Can't open database" << std::endl;
		conn.disconnect();

		return false;
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return false;
	}

}

bool testDBProviderLogger()
{
	using namespace DBProviderLogger;
	try
	{
		auto testLog = new DBLog();
		testLog->setPathAndOpen("../build/DBProvider.dir/log.txt");
		testLog->addLog(DEBUG, "test my log");
		testLog->addLog(DEBUG, "test my log one more time");

		startLog("../build/DBProvider.dir/global_log.txt");
		logDebug("test my global log");
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return false;
	}

	return true;
}

bool testCustomConnectionMethod(std::string credentials)
{
	try
	{
		auto *customConn = new DBConnection(credentials);
		customConn->setConnection();

		std::cout << "dbname= " << customConn->info.databaseName << std::endl;
		std::cout << "user= " << customConn->info.username << std::endl;
		std::cout << "password= " << customConn->info.password << std::endl;
		std::cout << "hostaddr= " << customConn->info.host << std::endl;
		std::cout << "port= " << customConn->info.portNumber << std::endl;

		delete customConn;
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return false;
	}

	return true;
}

int main()
{
	bool isStdConnectionWorks = testStandardConnectionMethod();
	bool isLogWorks = testDBProviderLogger();

	std::cout << "\nRUNNING: testCustomConnectionMethod" << std::endl;
	std::string creds = "Doors:doo:rc:127.0.0.1:5432";
	bool isCustomConnectionWorks = testCustomConnectionMethod(creds);
	if (isCustomConnectionWorks)
	{
		std::cout << "SUCCEEDED: testCustomConnectionMethod" << std::endl;
	}

	std::pair<std::vector<std::string>, std::string> result = ParsingTools::parseCredentials(creds);
	return 0;
}