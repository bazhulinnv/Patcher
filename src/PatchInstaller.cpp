#include <fstream>
#include <iostream>
#include <array>
#include <time.h>
#include <filesystem>
#include <direct.h>

#include "PatchInstaller/PatchInstaller.h"
#include "PatchInstaller/DependenciesChecker.h"
#include "Shared/Logger.h"
#include "PatchInstaller/FileParser.h"

using namespace PatcherLogger;

PatchInstaller::PatchInstaller() {}
PatchInstaller::~PatchInstaller() {}


/** The function checks the presence of objects in the database according to the list of objects specified in the file. */
bool PatchInstaller::checkObjectsForExistenceFromFile(std::string nameOfFile, DBProvider *dbProvider) {
	DBProviderListParameters objectsNameAndType = FileParser::getResultOfParsing(nameOfFile);
	DependenciesChecker checker;
	bool result = checker.getCheck(objectsNameAndType, dbProvider);

	std::cerr << "CHECKING DEPENDENCIES PROCESS:\n";
	std::cerr << checker.getDataForLog();
	if (!result) {
		std::cerr << "Check failed. Some objects does not exist in current database.\n";
	}
	else {
		std::cerr << "Check completed succesful.\n";
	}
	checker.print();

	auto *infoLog = new Log();
	infoLog->setLogByPath("Temp/CheckingDependenciesErrors.log");
	infoLog->addLog(INFO, checker.getDataForLog());

	delete infoLog;
	return result;
}

/** When the method starts, the dependency check is considered successful. */
bool PatchInstaller::startInstallation() {
	system("Install.bat");
	std::ifstream errors("tempError.txt", std::ios::in);
	std::ifstream info("tempInfo.txt", std::ios::in);
	bool resultOfInstall = false;

	std::string dataForErrorLog;
	std::string dataForInfoLog;
	std::string buffer;
	
	std::cerr << "INSTALLATION PROCESS:\n";
	while (getline(info, buffer)) {
		dataForInfoLog += buffer;
		dataForInfoLog += "\n";
		std::cerr << buffer << "\n";
	}
	buffer = "";
	std::cerr << "INSTALLATION ERRORS:\n";
	while (getline(errors, buffer)) {
		dataForErrorLog += buffer;
		dataForErrorLog += "\n";
		std::cerr << buffer << "\n";
	}

	dataForErrorLog += "Installation completed";
	std::cerr << "Installation completed";
	if (dataForErrorLog == "") {
		resultOfInstall = true;
		dataForErrorLog += " without errors.\n";
		std::cerr << " without errors.\n";
	}
	else {
		dataForErrorLog += " with errors.\n";
		std::cerr << " with errors.\n";
	}

	auto *errorLog = new Log();
	errorLog->setLogByPath("Temp/InstallationErrors.log");
	errorLog->addLog(ERROR, dataForErrorLog);
	delete errorLog;

	auto *infoLog = new Log();
	errorLog->setLogByPath("Temp/InstallationInfo.log");
	errorLog->addLog(INFO, dataForInfoLog);
	delete infoLog;

	errors.close();
	info.close();
	remove("tempError.txt");
	remove("tempInfo.txt");
	return resultOfInstall;
}
