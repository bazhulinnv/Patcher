#include <iostream>

#include "PatchInstaller/DependenciesChecker.h"

DependenciesChecker::DependenciesChecker() {};
DependenciesChecker::~DependenciesChecker() {};

bool DependenciesChecker::check(DependenciesChecker &checker, std::list<std::tuple<std::string, std::string, std::string>> objectsNameAndType, DBProvider dbProvider) {
	//std::cout << " in DependenciesChecker::check " << objectsNameAndType.size() << "\n";
	for (auto i : objectsNameAndType) {
		//bool doesCurrentObjectExist = dbProvider.isCurrentObjectExist(std::get<0>(i), std::get<1>(i), std::get<2>(i));
		//std::cout << "does current object exist? " << doesCurrentObjectExist;
		bool doesCurrentObjectExist = false;
		checker.allObjectsExists = checker.allObjectsExists && doesCurrentObjectExist;
		checker.existenceEachObject.push_back(doesCurrentObjectExist);
		//std::cout << " existence added" << doesCurrentObjectExist << "\n";
	}
	return checker.allObjectsExists;

}

void DependenciesChecker::printExistenceOfEachObject(DependenciesChecker &checker) {
	std::list <bool> ::iterator iterator;
	//std::cout << checker.existenceEachObject.size() << "\n";
	for (auto x : checker.existenceEachObject) {
		//std::cout << checker.existenceEachObject.size() << "\n";
		std::cout << x;
	}
}