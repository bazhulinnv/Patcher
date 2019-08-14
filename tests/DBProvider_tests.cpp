#include "DBProvider/DBProvider.h"
#include "Shared/Logger.h"
#include "Shared/TextTable.h"

#include <pqxx/pqxx>
#include <iostream>
#include <functional>

using namespace std;

bool test_sequenceExists(DBProvider* dbProv)
{
	return dbProv->sequenceExists("public", "math_blob_id_seq");
}

bool test_tableExists(DBProvider* dbProv)
{
	return dbProv->tableExists("public", "errors");
}

bool test_PrintTableList(DBProvider* dbProv)
{
	pqxx::result queryResult;

	string requestSQL = "SELECT * FROM pg_catalog.pg_tables "
		"WHERE schemaname != 'pg_catalog' "
		"AND schemaname != 'information_schema'; ";
	try
	{
		// Get all tables from database
		queryResult = dbProv->query(requestSQL);
	}
	catch (const std::exception& e)
	{
		cerr << e.what() << endl;
		return false;
	}

	if (queryResult.empty())
	{
		return false;
	}

	// Print objs using TextTable
	TextTable result('-', '|', '+');
	result.add(" TABLE NAME ");
	result.endOfRow();

	cout << "\nList of all available tables:" << endl;
	for (pqxx::result::const_iterator row = queryResult.begin();
		 row != queryResult.end(); ++row)
	{
		result.add(row["tablename"].as<string>(), green);
		result.endOfRow();
	}

	result.setAlignment(2, TextTable::Alignment::RIGHT);
	std::cout << result;
	return true;
}

bool test_GetFunctions(DBProvider* dbProv)
{
	return false;
}

bool test_GetFunctionParameters(DBProvider* dbProv)
{
	return false;
}

bool test_GetTriggers(DBProvider* dbProv)
{
	return false;
}

bool test_PrintFunctions(DBProvider* dbProv)
{
	return false;
}

bool test_PrintFunctionParameters(DBProvider* dbProv)
{
	return false;
}

bool test_PrintTriggers(DBProvider* dbProv)
{
	return false;
}

bool test_PrintObjectsAsTable(DBProvider* dbProv)
{
	vector<ObjectData> objs;

	try
	{
		// Get all objects from database
		objs = dbProv->getObjects();
	}
	catch (const std::exception& e)
	{
		cerr << e.what() << endl;
		return false;
	}

	if (objs.empty())
	{
		cout << "\nCould not get objects from database." << endl;
		cout << "[No exception thrown]" << endl;
		return false;
	}

	// Print objs using TextTable
	TextTable result('-', '|', '+');
	result.add("NAME");
	result.add("TYPE");
	result.add("SCHEME");
	result.add("PARAMS");
	result.endOfRow();

	for (auto i = 0; i < objs.size(); ++i)
	{
		result.add(objs[i].name);
		result.add(objs[i].type);
		result.add(objs[i].scheme);

		std::string acc = "";
		if (!objs[i].paramsVector.empty())
		{
			for (const auto& param : objs[i].paramsVector)
			{
				acc += param + " ";
			}
		}

		result.add(acc);
		result.endOfRow();
	}

	result.setAlignment(2, TextTable::Alignment::RIGHT);
	std::cout << result;
	return true;
}

bool test_PqxxConnection()
{
	try
	{
		pqxx::connection conn("dbname = Doors user = doo password = rs hostaddr = 127.0.0.1 port = 5432");
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

bool test_Logger()
{
	try
	{
		using namespace PatcherLogger;

		// create two different logs
		auto* testLog = new Log();

		// write some messages to both
		testLog->setLogByPath("../build/DBProvider.dir/log_test.txt");
		testLog->addLog(DEBUG, "RUNNING: testLogger() from DBProvider_tests");
		testLog->addLog(DEBUG, "TEST - 01");

		auto* logInStdDir = new Log();

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

//bool test_CustomConnection()
//{
//	try
//	{
//		auto *dbConn = new Connection("Doors:doo:rc:127.0.0.1:5432");
//		dbConn->setConnection();
//
//		cout << "dbname= " << dbConn->info.databaseName << endl;
//		cout << "user= " << dbConn->info.username << endl;
//		cout << "password= " << dbConn->info.password << endl;
//		cout << "hostaddr= " << dbConn->info.host << endl;
//		cout << "port= " << dbConn->info.portNumber << endl;
//
//		delete dbConn;
//	}
//	catch (const exception& e)
//	{
//		cerr << e.what() << endl;
//		return false;
//	}
//
//	return true;
//}

bool test_PrintObjectsData(DBProvider* dbProv)
{
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

	auto result = dbProv->query(getObjects);
	printObjectsData(result);
	return true;
}

bool test_PrintObjectsDataShared(shared_ptr<DBProvider> dbProv)
{
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

	auto result = dbProv->query(getObjects);
	printObjectsData(result);
	return true;
}

void runTestFunction(const string& testInfo, function<bool(DBProvider*)> sut, DBProvider* dbProv)
{
	cout << yellow;
	cout << "\nRUNNING: " << reset << testInfo << endl;

	if (!sut(dbProv))
	{
		cout << red;
		cout << "FAILED: ";
		cout << yellow;
		cout << testInfo << reset << endl;
		return;
	}

	cout << green;
	cout << "SUCCEEDED: " << reset;
	cout << testInfo << endl;
	cout << endl;
};

void runTestFunctionShared(const string& testInfo, function<bool(shared_ptr<DBProvider>)> sut, shared_ptr<DBProvider> dbProv)
{
	cout << yellow;
	cout << "\nRUNNING: " << reset << testInfo << endl;

	if (!sut(dbProv))
	{
		cout << red;
		cout << "FAILED: ";
		cout << yellow;
		cout << testInfo << reset << endl;
		return;
	}

	cout << green;
	cout << "SUCCEEDED: " << reset;
	cout << testInfo << endl;
	cout << endl;
};

void runAllTests(vector<pair<const string, function<bool(DBProvider*)>>> providerTests, DBProvider* dbProv)
{
	cout << yellow;
	cout << "\t##########\t" << "\tTESTING STARTED\t" << "\t##########" << reset << endl;

	for (auto& test : providerTests)
	{
		runTestFunction(test.first, test.second, dbProv);
	}

	cout << yellow;
	cout << "\t##########\t" << "\tTESTING FINISHED.\t" << "\t##########" << reset << endl;
};

void runAllTestsShared(vector<pair<const string, function<bool(shared_ptr<DBProvider>)>>> providerTests, shared_ptr<DBProvider> dbProv)
{
	cout << yellow;
	cout << "\t##########\t" << "\tTESTING STARTED\t" << "\t##########" << reset << endl;

	for (auto& test : providerTests)
	{
		runTestFunctionShared(test.first, test.second, dbProv);
	}

	cout << yellow;
	cout << "\t##########\t" << "\tTESTING FINISHED.\t" << "\t##########" << reset << endl;
};

void runAllSimpleTests(vector<pair<const string, function<bool()>>> simpleTests)
{
	cout << yellow;
	cout << "\t##########\t" << "RUNNING SIMPLE TESTS" << "\t##########" << reset << endl;

	for (auto& test : simpleTests)
	{
		cout << yellow;
		cout << "\nRUNNING: " << reset << test.first << endl;

		if (!test.second())
		{
			cout << red;
			cout << "FAILED: ";
			cout << yellow;
			cout << test.first << reset << endl;
			return;
		}

		cout << green;
		cout << "SUCCEEDED: " << reset;
		cout << test.first << endl;
		cout << endl;
	}

	cout << yellow;
	cout << "\t##########\t" << "FINISHED SIMPLE TESTS." << "\t##########" << reset << endl;
};

int main()
{
	string sql_publicRoleAction = "SELECT * FROM public.role_action";

	string loginData = "127.0.0.1:5432:Doors:doo:rs";
	// DBProvider* dbProvider = new DBProvider(loginData);
	auto dbProviderShared = make_shared<DBProvider>(loginData);

	// < test_info, pointer_to_test_function >
	vector<pair<const string, function<bool()>>> simpleTests;
	simpleTests.push_back(make_pair("Standart Test : make shure pqxx::connection works", test_PqxxConnection));
	simpleTests.push_back(make_pair("Test PatchLogger::Log : write to different logs", test_Logger));
	/*simpleTests.push_back(make_pair("Test Connection : connect to 'Doors'", test_CustomConnection));*/

	vector<pair<const string, function<bool(DBProvider*)>>> providerTest;
	vector<pair<const string, function<bool(shared_ptr<DBProvider>)>>> providerTestShared;
	providerTest.push_back(make_pair("Test printing raw data : printing raw objects data", test_PrintObjectsData));
	providerTest.push_back(make_pair("Test printing data in table : printing objects data wrapped in table", test_PrintObjectsAsTable));
	providerTest.push_back(make_pair("Test printing all tables : printing tables in io, public, common, secure", test_PrintTableList));
	providerTest.push_back(make_pair("Test : check table existance", test_tableExists));
	providerTest.push_back(make_pair("Test : check sequence existance", test_sequenceExists));

	providerTest.push_back(make_pair("Test functions", test_GetFunctions));
	providerTest.push_back(make_pair("Test function parameters", test_GetFunctionParameters));
	providerTest.push_back(make_pair("Test triggers", test_GetTriggers));
	providerTest.push_back(make_pair("Test printing functions", test_PrintFunctions));
	providerTest.push_back(make_pair("Test printing function parameters", test_PrintFunctionParameters));
	providerTest.push_back(make_pair("Test printing triggers", test_PrintTriggers));

	// Shared tests
	providerTestShared.push_back(make_pair("Test printing raw data : printing raw objects data", test_PrintObjectsDataShared));

	// Run all simple test functions
	runAllSimpleTests(simpleTests);

	runAllTestsShared(providerTestShared, dbProviderShared);

	// Run all test functions for DBProvider
	//runAllTests(providerTest, dbProvider);

	return 0;
}
