#ifndef DEPENDENCIESCHECKER_H
#define DEPENDENCIESCHECKER_H

#include <iostream>
#include <unordered_map>

#include "DBProvider/DBProvider.h"

class DependenciesChecker {
public:
	DependenciesChecker();
	~DependenciesChecker();

	std::list<bool> getListWithExistenceEachObject();
	bool getIfAllObjectsExists();
	std::string getDataForLog();

	bool getCheck(DBProviderListParameters &objectsNameAndType, DBProvider *dbProvider);
	void print();

private:
	std::list<bool> existenceEachObject;
	bool allObjectsExists;
	std::string dataForLog;
	bool DependenciesChecker::check(DBProviderListParameters &objectsNameAndType, DBProvider *dbProvider);
	void DependenciesChecker::printExistenceOfEachObject();
};
#endif