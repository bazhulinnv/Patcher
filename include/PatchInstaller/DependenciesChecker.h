#ifndef DEPENDENCIESCHECKER_H
#define DEPENDENCIESCHECKER_H

#include <iostream>
#include <unordered_map>

#include "DBProvider/DBProvider.h"

class DependenciesChecker {
public:
	DependenciesChecker();
	~DependenciesChecker();
	//getters
	std::list<bool> getListWithExistenceEachObject();
	bool getIfAllObjectsExists();
	std::string getDataForLog();

	//wrappers for private methods: check dependencies list of objects; print existence of each object for gui
	bool getCheck(DBObjects &objectParameters, DBProvider *dbProvider);
	void print();

private:
	std::list<bool> existenceEachObject;
	bool allObjectsExists;
	std::string dataForLog;
	bool DependenciesChecker::check(DBObjects &objectsParameters, DBProvider *dbProvider);
	void DependenciesChecker::printExistenceOfEachObject();
};
#endif