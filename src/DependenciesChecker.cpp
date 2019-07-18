#include <iostream>

#include "PatchInstaller/DependenciesChecker.h"

DependenciesChecker::DependenciesChecker() {};
DependenciesChecker::~DependenciesChecker() {}

bool DependenciesChecker::check(DependenciesChecker &checker, std::list<std::pair<std::string, std::string>> objectsNameAndType, DBProvider dbProvider) {
	std::list<std::pair<std::string, std::string>>::iterator i;
	for (i = objectsNameAndType.begin(); i != objectsNameAndType.end(); i++) {
		//std::cout << i->first << " " << i->second << "\n";
		bool doesCurrentObjectExist = dbProvider.isCurrentObjectExist(i->first, i->second);
		//std::cout << "does current object exist? " << doesCurrentObjectExist;
		checker.allObjectsExists = checker.allObjectsExists && doesCurrentObjectExist;
		checker.existenceEachObject.push_back(doesCurrentObjectExist);
		//std::cout << " existence added" << doesCurrentObjectExist << "\n";
	}
	return checker.allObjectsExists;

}

void DependenciesChecker::printExistenceOfEachObjectForGUI(DependenciesChecker &checker) {
	std::list <bool> ::iterator iterator;
	for (auto x : checker.existenceEachObject) {
		std::cout << x;
	}
}