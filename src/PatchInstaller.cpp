#include <cstdio>
#include <PatchInstaller/PatchInstaller.h>
#include "DBProvider/DBProvider.h"
#include "PatchInstaller/PatchInstaller.h"
#include <String>
#include <fstream>
#include <iostream>

using namespace std;

PatchInstaller::PatchInstaller() {}
PatchInstaller::~PatchInstaller() {}

bool PatchInstaller::checkObjectsForExistence(std::string nameOfFile)
{
	bool result = true;
	DBProvider dbProvider;
	ifstream dependencies;
	dependencies.open(nameOfFile, std::ios::in);
	//Generate file, when for each object write exist it, or not
	ofstream objectsExistence("ObjectsExistence.txt");

	std::cout << "Created file objectsExistence!\n";
	std::string buffer;

	while (getline(dependencies, buffer)) {
		//parsing of file with list of objects
		std::string objectName("");
		std::string objectType("");

		//dependencies.getline(buff, sizeof(buff));
		dependencies >> objectName >> objectType;
		std::cout << objectName << " " << objectType << "\n";

		result = result && dbProvider.isCurrentObjectExist(objectName, objectType);

		if (!dbProvider.isCurrentObjectExist(objectName, objectType)) {
			//writing to logging file with existence of objects
			std::cout << "Write in objectExistingFile!\n";
			objectsExistence << objectName <<  " " << objectType << " " << 
				dbProvider.isCurrentObjectExist(objectName, objectType) << "\n";
		}
	}

	dependencies.close();
	objectsExistence.close();

	return result;
}



bool startInstallation(FILE* fileWithDependencies, std::string objectsAndScriptsDirectory, 
					std::string installationScriptDirectory, PatchInstaller patchInstaller) {
	//Take the list of dependencies and check it for the presence in database
	/*if (patchInstaller.checkObjectsForExistence(fileWithDependencies)) {
		//Check succeed
		//Start installation script
		FILE* installationSctipt;
		//Generate logging file with installation mistakes
	}
	else {
		//Check failed (some object does not exist)
		return false;
	}*/
	return false;
}
