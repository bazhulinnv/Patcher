#include <cstdio>
#include <String>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <array>
#include <list>

#include "PatchInstaller/PatchInstaller.h"
#include "DBProvider/DBProvider.h"
#include "PatchInstaller/PatchInstaller.h"


using namespace std;

PatchInstaller::PatchInstaller() {}
PatchInstaller::~PatchInstaller() {}

void PatchInstaller::passParametersToDBProvider(char *parameters, DBProvider dbProvider) {
	dbProvider.connect(parameters);
}

/** The function checks the presence of objects in the database according to the list of objects specified in the file. */
bool PatchInstaller::checkObjectsForExistence(std::string nameOfFile, DBProvider dbProvider)
{
	bool result = true;
	ifstream dependencies(nameOfFile, ios::in);
	ofstream objectsExistence("ObjectsExistence.log");

	std::string buffer("");
	std::string objectName("");
	std::string objectType("");
	list <bool> objectsExistenceForGUI;

	//Try to read first string from file
	dependencies >> objectName >> objectType;

	if ((objectName != "") && (objectType != "")) {
		while (getline(dependencies, buffer)) {
			result = result && dbProvider.isCurrentObjectExist(objectName, objectType);
			objectsExistence << objectName << " " << objectType << " ";
			if (!dbProvider.isCurrentObjectExist(objectName, objectType)) {
				objectsExistence << "not ";
			}
	
			objectsExistence << "exists\n";

			objectsExistenceForGUI.push_back(dbProvider.isCurrentObjectExist(objectName, objectType));
			dependencies >> objectName >> objectType;
		}
		objectsExistence << objectName << " " << objectType << " ";
		if (!dbProvider.isCurrentObjectExist(objectName, objectType)) {
			objectsExistence << "not ";
		}
		objectsExistence << "exists\n";

		objectsExistenceForGUI.push_back(dbProvider.isCurrentObjectExist(objectName, objectType));

		copy(objectsExistenceForGUI.begin(), objectsExistenceForGUI.end(), ostream_iterator<int>(cout, ""));
	}

	dependencies.close();
	objectsExistence.close();

	return result;
}

/** Figuring out the operating system. */
#if !defined(__WIN32__) && (defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__))
#  define __WIN32__
#endif

#if defined(__unix__) || defined(__unix) || defined(_UNIXWARE7)
#define __unix__
#endif

/** When the method starts, the dependency check is considered successful. */
bool PatchInstaller::startInstallation() {
	std::string command("dir");
	std::array<char, 128> buffer;
	std::string result;

#if defined(__WIN32__)
	FILE* pipe = _popen(command.c_str(), "r");
	if (!pipe) {
		std::cerr << "Couldn't start command." << std::endl;
		return 0;
	}
	while (fgets(buffer.data(), 128, pipe) != NULL) {
		result += buffer.data();
	}
	auto returnCode = _pclose(pipe);
#endif

#if (defined(__unix__)) 
	FILE* pipe = popen(command.c_str(), "r");
	if (!pipe) {
		std::cerr << "Couldn't start command." << std::endl;
		return 0;
	}
	while (fgets(buffer.data(), 128, pipe) != NULL) {
		result += buffer.data();
	}
	auto returnCode = pclose(pipe);
#endif

	ofstream file("Installing.log");
	file << result;
	return true;
}

