#include "DBProvider/DBProvider.h"
#include <memory>
#include <functional>

namespace TestUtility
{
	void runTest(const string& testInfo, function<bool(DBProvider*)> sut, DBProvider* dbProv);

	void runTest(const string& testInfo, function<bool(shared_ptr<DBProvider>)> sut, shared_ptr<DBProvider> dbProv);

	void runAll(vector<pair<const string, function<bool(DBProvider*)>>> providerTests, DBProvider* dbProv);

	void runAll(vector<pair<const string, function<bool(shared_ptr<DBProvider>)>>> providerTests,
				shared_ptr<DBProvider> dbProv);

	void runSimpleTests(vector<pair<const string, function<bool()>>> simpleTests);
}
