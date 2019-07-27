#include "DBProvider/DBProvider.h"
#include "Shared/Logger.h"
#include "Shared/TextTable.h"
#include <iostream>
#include <pqxx/pqxx>
#include <functional>

using namespace std;

bool testPqxxConnection()
{
	try
	{
		pqxx::connection conn("dbname = Doors user = doo password = rc hostaddr = 127.0.0.1 port = 5432");
		if (!conn.is_open())
		{
			cout << "Can't open database" << endl;
			cout << "[No exception thrown]" << endl;
			return false;
		}

		cout << "Opened database successfully: " << conn.dbname() << endl;
		cout << "Disconnecting from: " << conn.dbname() << endl;
		conn.disconnect();
	}
	catch (const exception& e)
	{
		cerr << e.what() << endl;
		return false;
	}

	return true;
}

bool testLogger()
{
	try
	{
		using namespace PatcherLogger;

		// create two different logs
		auto *testLog = new Log();

		// write some messages to both
		testLog->setLogByPath("../build/DBProvider.dir/log_test.txt");
		testLog->addLog(DEBUG, "RUNNING: testLogger() from DBProvider_tests");
		testLog->addLog(DEBUG, "TEST - 01");

		auto *logInStdDir = new Log();

		logInStdDir->setLogByName("new_log.log");
		logInStdDir->addLog(DEBUG, "RUNNING: testLogger() from DBProvider_tests");
		logInStdDir->addLog(DEBUG, "TEST - 02");

		// init global log 
		startGlobalLog("../build/DBProvider.dir/global_log.txt");
		
		// write some messages to global log
		logDebug("Test GLOBAL LOG");

		// delete logs (not files)
		delete logInStdDir;
		delete testLog;

		stopGlobalLog();
	}
	catch (const exception& e)
	{
		cerr << e.what() << endl;
		return false;
	}

	return true;
}

bool testCustomConnection()
{
	try
	{
		auto *dbConn = new DBConnection("Doors:doo:rc:127.0.0.1:5432");
		dbConn->setConnection();

		cout << "dbname= " << dbConn->info.databaseName << endl;
		cout << "user= " << dbConn->info.username << endl;
		cout << "password= " << dbConn->info.password << endl;
		cout << "hostaddr= " << dbConn->info.host << endl;
		cout << "port= " << dbConn->info.portNumber << endl;

		delete dbConn;
	}
	catch (const exception& e)
	{
		cerr << e.what() << endl;
		return false;
	}

	return true;
}

bool testPrintObjectsData()
{
	auto *dbProvider = new DBProvider("Doors:doo:rc:127.0.0.1:5432");
	string getObjects =
		"SELECT /*sequences */"
		"f.sequence_schema AS obj_schema,"
		"f.sequence_name AS obj_name,"
		"'sequence' AS obj_type "
		"FROM information_schema.sequences f "
		"UNION ALL "
		"SELECT /*tables */ "
		"f.table_schema AS obj_schema,"
		"f.table_name AS obj_name,"
		"'tables' AS obj_type "
		"FROM information_schema.tables f "
		"WHERE f.table_schema in"
		"('public', 'io', 'common', 'secure');";
	
	auto result = dbProvider->query(getObjects);
	printObjectsData(result);

	delete dbProvider;
	return true;
}

void runTestFunction(const string &testInfo, function<bool()> sut)
{
	cout << "\nRUNNING: " << testInfo << endl;
	
	if (!sut())
	{
		cout << "FAILED: " << testInfo << endl;
		//throw exception("\nSome test(s) failed.\n");
		return;
	}
	
	cout << "SUCCEEDED: " << testInfo << endl;
	cout << endl;
};

void runAllTests(vector<pair<const string, function<bool()>>> allTests)
{
	cout << "\t##########\t" << "TESTING STARTED" << "\t##########" << endl;
	for (auto& test : allTests)
	{
		runTestFunction(test.first, test.second);
	}
	
	cout << "\t##########\t" << "TESTING FINISHED." << "\t##########" << endl;
};


int main()
{		
	string sql_publicRoleAction =	"SELECT * FROM public.role_action";
	string sql_listTables = "SELECT * FROM pg_catalog.pg_tables "
							"WHERE schemaname != 'pg_catalog' "
							"AND schemaname != 'information_schema'; ";
	
	string sql_getObjects =	"SELECT /*sequences */"
								"f.sequence_schema AS obj_schema,"
								"f.sequence_name AS obj_name,"
								"'sequence' AS obj_type "
							"FROM information_schema.sequences f "
							"UNION ALL "
							"SELECT /*tables */ "
								"f.table_schema AS obj_schema,"
								"f.table_name AS obj_name,"
								"'tables' AS obj_type "
							"FROM information_schema.tables f "
							"WHERE f.table_schema in"
								"('public', 'io', 'common', 'secure');";
	
	string loginData = "Doors:doo:rc:127.0.0.1:5432";
	auto* dbProvider = new DBProvider(loginData);

	// Init vectors with < test_info, pointer_to_test_function >
	vector<pair<const string, function<bool()>>> testList;
	
	testList.push_back(make_pair("Standart Test : make shure pqxx::connection works", testPqxxConnection));
	testList.push_back(make_pair("Test PatchLogger::Log : write to different logs", testLogger));
	testList.push_back(make_pair("Test DBConnection : connect to 'Doors'", testCustomConnection));
	testList.push_back(make_pair("Test PrintObjectsData : printing all objects raw data", testPrintObjectsData));

	// Run all test functions
	runAllTests(testList);

	// Print all tables in database
	auto queryResult = dbProvider->query(sql_listTables);

	cout << "\nList of all available tables:" << endl;
	for (pqxx::result::const_iterator row = queryResult.begin();
		row != queryResult.end(); ++row)
	{
		cout << row["tablename"].as<string>() << "\t" << endl;
	}

	queryResult.clear();

	// Print all objects to std::cout
	dbProvider->printObjectsData();

	delete dbProvider;
	return 0;
}