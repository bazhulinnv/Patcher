#include "Shared/TestUtility.h"
#include "Shared/TextTable.h"
#include <iostream>

using namespace std;

void TestUtility::runTest(const string& testInfo, function<bool(DBProvider*)> sut, DBProvider* dbProv)
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
}

void TestUtility::runTest(const string& testInfo, function<bool(shared_ptr<DBProvider>)> sut,
                          shared_ptr<DBProvider> dbProv)
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
}

void TestUtility::runAll(vector<pair<const string, function<bool(DBProvider*)>>> providerTests, DBProvider* dbProv)
{
	cout << yellow;
	cout << "\t##########\t" << "\tTESTING STARTED\t" << "\t##########" << reset << endl;

	for (auto& test : providerTests)
	{
		runTest(test.first, test.second, dbProv);
	}

	cout << yellow;
	cout << "\t##########\t" << "\tTESTING FINISHED.\t" << "\t##########" << reset << endl;
}

void TestUtility::runAll(vector<pair<const string, function<bool(shared_ptr<DBProvider>)>>> providerTests,
                         shared_ptr<DBProvider> dbProv)
{
	cout << yellow;
	cout << "\t##########\t" << "\tTESTING STARTED\t" << "\t##########" << reset << endl;

	for (auto& test : providerTests)
	{
		runTest(test.first, test.second, dbProv);
	}

	cout << yellow;
	cout << "\t##########\t" << "\tTESTING FINISHED.\t" << "\t##########" << reset << endl;
}

void TestUtility::runSimpleTests(vector<pair<const string, function<bool()>>> simpleTests)
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
}
