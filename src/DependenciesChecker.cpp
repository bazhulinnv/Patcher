#include <iostream>

#include "PatchInstaller/PatchInstaller.h"
#include "PatchInstaller/DependenciesChecker.h"

DependenciesChecker::DependenciesChecker() {
	allObjectsExists = true;
};
DependenciesChecker::~DependenciesChecker() {
};

bool DependenciesChecker::check(DBProviderListParameters &objectsNameAndType, DBProvider &dbProvider) {
	for (auto i : objectsNameAndType) {
		bool doesCurrentObjectExist = dbProvider.doesCurrentObjectExists(std::get<0>(i), std::get<1>(i), std::get<2>(i));
		allObjectsExists = allObjectsExists && doesCurrentObjectExist;
		existenceEachObject.emplace_back(doesCurrentObjectExist);
		
		dataForLog += std::get<0>(i) + " " + std::get<1>(i) + " " + std::get<2>(i) + " ";
		if (!doesCurrentObjectExist) {
			dataForLog += "not ";
		}
		dataForLog += "exists\n";
	}
	return allObjectsExists;

}

void DependenciesChecker::printExistenceOfEachObject() {
	std::list <bool> ::iterator iterator;
	for (auto x : existenceEachObject) {
		std::cout << x;
	}
}