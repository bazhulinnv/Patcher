#include <iostream>

#include "PatchInstaller/DependenciesChecker.h"

DependenciesChecker::DependenciesChecker() {
	allObjectsExists = true;
};
DependenciesChecker::~DependenciesChecker() {
};

bool DependenciesChecker::check(std::list<std::tuple<std::string, std::string, std::string>> &objectsNameAndType, DBProvider &dbProvider) {
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
	//std::cout << checker.existenceEachObject.size() << "\n";
	for (auto x : existenceEachObject) {
		//std::cout << checker.existenceEachObject.size() << "\n";
		std::cout << x;
	}
}