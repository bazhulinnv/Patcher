#include "DBProvider/DBProvider.h"
#include "Shared/Logger.h"
#include <iostream>
#include <pqxx/pqxx>

bool testPqxxConnectionMethod()
{
	using namespace pqxx;

	try
	{
		connection conn("dbname = Doors user = doo password = rc hostaddr = 127.0.0.1 port = 5432");

		if (conn.is_open())
		{
			std::cout << "Opened database successfully: " << conn.dbname() << std::endl;
			std::cout << "Disconnecting from: " << conn.dbname() << std::endl;
			conn.disconnect();
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

bool testLogger()
{
	using namespace PatcherLogger;
	try
	{
		auto *testLog = new Log();
		testLog->setLogByPath("../build/DBProvider.dir/log_test.txt");
		
		testLog->addLog(DEBUG, "RUNNING: testLogger() from DBProvider_tests");
		testLog->addLog(DEBUG, "TEST - 01");

		auto *logInStdDir = new Log();
		logInStdDir->setLogByName("new_log.log");

		logInStdDir->addLog(DEBUG, "RUNNING: testLogger() from DBProvider_tests");
		logInStdDir->addLog(DEBUG, "TEST - 02");

		startGlobalLog("../build/DBProvider.dir/global_log.txt");
		logDebug("Test GLOBAL LOG");
		delete logInStdDir;
		delete testLog;
		stopGlobalLog();
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

bool testPrintObjectsData(std::string creds)
{
	auto *dbProvider = new DBProvider(creds);
	std::string getObjects =
		"SELECT /*sequences */\
				f.sequence_schema AS obj_schema, f.sequence_name AS obj_name, 'sequence' AS obj_type\
				from information_schema.sequences f\
			UNION ALL\
			SELECT /*tables */\
				f.table_schema AS obj_schema, f.table_name AS obj_name, 'tables' AS obj_type\
				from information_schema.tables f\
			WHERE f.table_schema in('public', 'io', 'common', 'secure')";
	
	auto resOfQuery = dbProvider->query(getObjects);

	printObjectsData(resOfQuery);
	delete dbProvider;
	return true;
}

int main()
{
	std::string creds = "Doors:doo:rc:127.0.0.1:5432";

	std::cout << "\nRUNNING: Test Logger" << std::endl;
	if (testLogger())
	{
		std::cout << "SUCCEEDED: Test Logger" << std::endl;
	}

	std::cout << "\nRUNNING: Test pqxx connection" << std::endl;
	if (testPqxxConnectionMethod())
	{
		std::cout << "SUCCEEDED: Test pqxx connection" << std::endl;
	}

	std::cout << "\nRUNNING: Test custom connection method" << std::endl;
	if (testCustomConnectionMethod(creds))
	{
		std::cout << "SUCCEEDED: Test custom connection method" << std::endl;
	}
	
	std::cout << "\nRUNNING: Test print objects data method" << std::endl;
	if (testPrintObjectsData(creds))
	{
		std::cout << "SUCCEEDED: Test print objects data method" << std::endl;
	}
	
	return 0;
}