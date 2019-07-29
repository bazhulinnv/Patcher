#include <iostream>

#include "PatchInstaller/PatchInstaller.h"
#include "PatchInstaller/DependenciesChecker.h"

//Constructor and destructor
DependenciesChecker::DependenciesChecker() {
	allObjectsExists = true;
};

DependenciesChecker::~DependenciesChecker() {
};

//Getters
std::list<bool> DependenciesChecker::getListWithExistenceEachObject() {
	return existenceEachObject;
}

bool DependenciesChecker::getIfAllObjectsExists() {
	return allObjectsExists;
}

std::string DependenciesChecker::getDataForLog() {
	return dataForLog;
}

bool DependenciesChecker::getCheck(DBProviderListParameters &objectsNameAndType, DBProvider *dbProvider) {
	return check(objectsNameAndType, dbProvider);
}

//Passing output in cout to GUI
void DependenciesChecker::printExistenceOfEachObject() {
	std::list <bool> ::iterator iterator;
	for (auto x : existenceEachObject) {
		std::cout << x;
	}
}

void DependenciesChecker::print()
{
	printExistenceOfEachObject();
}

//Private implementation of check
bool DependenciesChecker::check(DBProviderListParameters &objectsNameAndType, DBProvider *dbProvider) {
	for (auto i : objectsNameAndType) {
		bool doesCurrentObjectExist = dbProvider->doesCurrentObjectExists(std::get<0>(i), std::get<1>(i), std::get<2>(i));
		if (!doesCurrentObjectExist) {
			allObjectsExists = false;
		}
		existenceEachObject.emplace_back(doesCurrentObjectExist);
		
		dataForLog += std::get<0>(i) + " " + std::get<1>(i) + " " + std::get<2>(i) + " ";
		if (!doesCurrentObjectExist) {
			dataForLog += "not ";
		}
		dataForLog += "exists\n";
	}
	return allObjectsExists;

}
