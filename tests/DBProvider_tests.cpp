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
  
	bool isStdConnectionWorks = testStandardConnectionMethod();
	// bool isLogWorks = testDBProviderLogger();

	//std::cout << "\nRUNNING: Test custom connection method" << std::endl;
	//
	//if (testCustomConnectionMethod(creds))
	//{
	//	std::cout << "SUCCEEDED: Test custom connection method" << std::endl;
	//}
	//
	//std::cout << "\nRUNNING: Test print objects data method" << std::endl;
	//if (testPrintObjectsData(creds))
	//{
	//	std::cout << "SUCCEEDED: Test print objects data method" << std::endl;
	//}
	
	//std::string getObjects =
	//	"SELECT /*sequences */\
	//			f.sequence_schema AS obj_schema, f.sequence_name AS obj_name, 'sequence' AS obj_type\
	//			from information_schema.sequences f\
	//		UNION ALL\
	//		SELECT /*tables */\
	//			f.table_schema AS obj_schema, f.table_name AS obj_name, 'tables' AS obj_type\
	//			from information_schema.tables f\
	//		WHERE f.table_schema in('public', 'io', 'common', 'secure')";

	//auto resOfQuery = dbProvider->query(getObjects);
	//std::string q = "SELECT * FROM public.role_action";
	//
	//std::string createTable = "CREATE TABLE account("
	//	"user_id serial PRIMARY KEY,"
	//	"username VARCHAR(50) UNIQUE NOT NULL,"
	//	"password VARCHAR(50) NOT NULL,"
	//	"email VARCHAR(355) UNIQUE NOT NULL,"
	//	"created_on TIMESTAMP NOT NULL,"
	//	"last_login TIMESTAMP"
	//	"); ";
	//
	//std::string k = "SELECT * FROM pg_catalog.pg_tables WHERE schemaname != 'pg_catalog' AND schemaname != 'information_schema'; ";
	//
	//resOfQuery = dbProvider->query(k);
	//
	//pqxx::result::const_iterator row;
	//
	//for (row = resOfQuery.begin(); row != resOfQuery.end(); ++row)
	//{
	//	std::cout << row["tablename"].as<std::string>() << "\t" << std::endl;
	//}


	//for (	row = resOfQuery.begin();
	//		row != resOfQuery.end();
	//		++row)
	//{
	//	std::cout
	//		<< row["obj_name"].as<std::string>() << "\t"
	//		<< row["obj_type"].as<std::string>() << "\t"
	//		<< row["obj_schema"].as<std::string>()
	//		<< std::endl;
	//}

	//std::vector<ObjectData> objects;

	//for (row = resOfQuery.begin();
	//	row != resOfQuery.end();
	//	++row)
	//{
	//	std::vector<std::string> parameters;
	//	objects.push_back(ObjectData(row["obj_name"].as<std::string>(), row["obj_type"].as<std::string>(), row["obj_schema"].as<std::string>(), parameters));
	//}
	
	auto *dbProvider = new DBProvider(creds);
	vector<ObjectData> objs = dbProvider->getObjects();

	std::cout << "|   NAME   " << "\t";
	std::cout << "|   TYPE   " << "\t";
	std::cout << "|  SCHEME  " << "\t";
	std::cout << "|  PARAMS  |" << std::endl;
	
	for (auto i = 0; i < objs.size(); ++i)
	{
		std::cout << objs[i].name << "\t";
		std::cout << objs[i].type << "\t";
		std::cout << objs[i].scheme << "\t";

		for (auto param : objs[i].paramsVector)
		{
			std::cout << param << "\t";
		}

		std::cout << std::endl;
	}

	delete dbProvider;
	return 0;
}