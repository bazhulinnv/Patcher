#include <cstdio>
#include <String>
//#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <array>
//#include <list>
#include <time.h>
#include <direct.h>

#include "PatchInstaller/PatchInstaller.h"
#include "PatchInstaller/DependenciesChecker.h"

PatchInstaller::PatchInstaller() {}
PatchInstaller::~PatchInstaller() {}

/** The function checks the presence of objects in the database according to the list of objects specified in the file. */
bool PatchInstaller::checkObjectsForExistenceFromFile(std::string nameOfFile, DBProvider dbProvider) {
	FileParser fileParser;
	std::list<std::pair<std::string, std::string>> objectsNameAndType = fileParser.parse("dependencies.txt");

	DependenciesChecker checker;
	checker.check(checker, objectsNameAndType, dbProvider);
	checker.printExistenceOfEachObjectForGUI(checker);

	return checker.allObjectsExists;
}


/** When the method starts, the dependency check is considered successful. */
bool PatchInstaller::startInstallation() {
	return true;
	
}

