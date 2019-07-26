#include <fstream>
#include <iostream>
#include <array>
#include <time.h>
#include <filesystem>
#include <direct.h>
//#include <dir.h>

#include "PatchInstaller/PatchInstaller.h"
#include "PatchInstaller/DependenciesChecker.h"
#include "Shared/Logger.h"

using namespace PatcherLogger;

PatchInstaller::PatchInstaller() {}
PatchInstaller::~PatchInstaller() {}


/** The function checks the presence of objects in the database according to the list of objects specified in the file. */
bool PatchInstaller::checkObjectsForExistenceFromFile(std::string nameOfFile, DBProvider dbProvider) {
	FileParser fileParser;
	std::list<std::tuple<std::string, std::string, std::string>> objectsNameAndType = fileParser.parse(nameOfFile);
	DependenciesChecker checker;
	bool result = checker.check(objectsNameAndType, dbProvider);
	std::cerr << "CHECKING DEPENDENCIES PROCESS:\n";
	std::cerr << checker.dataForLog;
	checker.printExistenceOfEachObject();

	auto *infoLog = new Log();
	infoLog->setLogByPath("Temp/CheckingDependenciesErrors.log");
	infoLog->addLog(INFO, checker.dataForLog);

	delete infoLog;
	return result;
}

/** When the method starts, the dependency check is considered successful. */
bool PatchInstaller::startInstallation() {
	system("Install.bat");
	std::ifstream errors("tempError.txt", ios::in);
	std::ifstream info("tempInfo.txt", ios::in);
	bool resultOfInstall = false;

	std::string dataForErrorLog;
	std::string dataForInfoLog;
	std::string buffer;
	
	while (getline(errors, buffer)) {
		dataForErrorLog += buffer;
		dataForErrorLog += "\n";
	}
	buffer = "";
	while (getline(info, buffer)) {
		dataForInfoLog += buffer;
		dataForInfoLog += "\n";
	}

	dataForErrorLog += "Installation completed";

	auto *errorLog = new Log();
	errorLog->setLogByPath("Temp/InstallationErrors.log");
	errorLog->addLog(ERROR, dataForErrorLog);
	delete errorLog;

	auto *infoLog = new Log();
	errorLog->setLogByPath("Temp/InstallationInfo.log");
	errorLog->addLog(INFO, dataForInfoLog);
	delete infoLog;

	if (dataForErrorLog == "") {
		resultOfInstall = true;
		dataForErrorLog += " without errors.\n";
	}
	else {
		dataForErrorLog += " with errors.\n";
	}

	std::cerr << "INSTALLATION PROCESS:\n";
	std::cerr << dataForInfoLog;
	std::cerr << "INSTALLATION ERRORS:\n";
	std::cerr << dataForErrorLog;

	errors.close();
	info.close();
	remove("tempError.txt");
	remove("tempInfo.txt");
	return resultOfInstall;
}
