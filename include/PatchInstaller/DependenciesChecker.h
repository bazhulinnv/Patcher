#ifndef DEPENDENCIESCHECKER_H
#define DEPENDENCIESCHECKER_H

#include <iostream>
#include <unordered_map>

#include "DBProvider/DBProvider.h"

class DependenciesChecker {
public:
	DependenciesChecker();
	~DependenciesChecker();
	std::list<bool> existenceEachObject;
	bool allObjectsExists;
	std::string dataForLog;
	bool DependenciesChecker::check(DependenciesChecker &checker, std::list<std::tuple<std::string, std::string, std::string>> objectsNameAndType, DBProvider dbProvider);
	void DependenciesChecker::printExistenceOfEachObject(DependenciesChecker &checker);
};
#endif