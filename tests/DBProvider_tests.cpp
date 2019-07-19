#include <DBProvider/DBProvider.h>
#include "../include/DBProvider/DBProviderLogger.h"
#include <iostream>
#include <pqxx/pqxx>

bool testStandardConnectionMethod()
{
	using namespace pqxx;
	using namespace std;

	try
	{
		connection conn("dbname = Doors user = doo password = rc hostaddr = 127.0.0.1 port = 5432");

		if (conn.is_open())
		{
			cout << "Opened database successfully: " << conn.dbname() << endl;
			return true;
		}
		
		cout << "Can't open database" << endl;
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
		auto customConn = new DBConnection(credentials);
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

	std::string creds = "Doors:doo:rc:127.0.0.1:5432";
	bool isCustomConnectionWorks = testCustomConnectionMethod(creds);

	std::pair<std::vector<std::string>, std::string> result = ParsingTools::parseCredentials(creds);
	return 0;
}