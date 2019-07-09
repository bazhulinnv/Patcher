#include <cstdio>
#include <String>
#include <fstream>
#include <iostream>

#include "PatchInstaller/PatchInstaller.h"
#include "DBProvider/DBProvider.h"
#include "PatchInstaller/PatchInstaller.h"


using namespace std;

PatchInstaller::PatchInstaller() {}
PatchInstaller::~PatchInstaller() {}

/* The function checks the presence of objects in the database according to the list of objects specified in the file. */
bool PatchInstaller::checkObjectsForExistence(std::string nameOfFile)
{
	bool result = true;
	DBProvider dbProvider;
	ifstream dependencies(nameOfFile, ios::in);
	ofstream objectsExistence("ObjectsExistence.txt");
	std::string buffer("");
	std::string objectName("");
	std::string objectType("");

	//Try to read first string from file
	dependencies >> objectName >> objectType;

	if ((objectName != "") && (objectType != "")) {
		while (getline(dependencies, buffer)) {
			result = result && dbProvider.isCurrentObjectExist(objectName, objectType);

			if (!dbProvider.isCurrentObjectExist(objectName, objectType)) {
				//writing to logging file with existence of objects
				objectsExistence << objectName << " " << objectType << " "
					<< dbProvider.isCurrentObjectExist(objectName, objectType) << "\n";
			}

			dependencies >> objectName >> objectType;
		}
		objectsExistence << objectName << " " << objectType << " " <<
			dbProvider.isCurrentObjectExist(objectName, objectType) << "\n";
	}

	dependencies.close();
	objectsExistence.close();

	return result;
}

/* When the method starts, the dependency check is considered successful. */
bool startInstallation(/* Installation script. */) {
	//Take the list of dependencies and check it for the presence in database
	//Check succeed
	//Start installation script
	//Generate logging file with installation mistakes
	return true;
}
