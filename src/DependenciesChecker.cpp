#include <iostream>

#include "PatchInstaller/PatchInstaller.h"
#include "PatchInstaller/DependenciesChecker.h"

//Constructor and destructor
DependenciesChecker::DependenciesChecker() {
	all_objects_exists = true;
};

DependenciesChecker::~DependenciesChecker() {
};

//Getters
std::list<bool> DependenciesChecker::getListWithExistenceEachObject() {
	return existence_each_object;
}

bool DependenciesChecker::getIfAllObjectsExists() {
	return all_objects_exists;
}

std::string DependenciesChecker::getDataForLog() {
	return data_for_log;
}

bool DependenciesChecker::getCheck(DBObjects &objects_parameters, DBProvider &dbProvider) {
	return check(objects_parameters, dbProvider);
}

//Passing output in cout to GUI
void DependenciesChecker::printExistenceOfEachObject() {
	std::list <bool> ::iterator iterator;
	for (auto x : existence_each_object) {
		std::cout << x;
	}
}

void DependenciesChecker::print() {
	//print list for GUI
	printExistenceOfEachObject();
}

//Private implementation of check
bool DependenciesChecker::check(DBObjects &objectsParameters, DBProvider &dbProvider) {
	for (auto i : objectsParameters) {
		bool does_current_object_exist = dbProvider.doesCurrentObjectExists(std::get<0>(i), std::get<1>(i), std::get<2>(i));
		if (!does_current_object_exist) {
			all_objects_exists = false;
		}
		//generate list of existence each object in format: 0 if object does not exist in current database, 1 if exists
		existence_each_object.emplace_back(does_current_object_exist);
		
		//generate data for log: object parameters + existence; example public cats table exists
		data_for_log += std::get<0>(i) + " " + std::get<1>(i) + " " + std::get<2>(i) + " ";
		if (!does_current_object_exist) {
			data_for_log += "does not exist\n";
		}
		else {
			data_for_log += "exists\n";
		}
	}
	//return true if all objects from lists exist, false otherwise
	return all_objects_exists;

}
