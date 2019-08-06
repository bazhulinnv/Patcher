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

/** Creates log for both parts of PatchInstaller. */
void createLog(std::string path, Level level, std::string data) {
	Log log;
	log.setLogByPath(path);
	log.addLog(level, data);

}

/** Checking dependencies part. */
void passCheckLogsForGui(std::string data, bool resultCheck) {
	std::cerr << "CHECKING DEPENDENCIES PROCESS:\n";
	std::cerr << data;
	if (!resultCheck) {
		std::cerr << "Check failed. Some objects does not exist in current database.\n";
	}
	else {
		std::cerr << "Check completed succesful.\n";
	}
}

/** Main method of checking dependencies.
	Checks the presence of objects in the database according to the list of objects specified in the file. */
bool PatchInstaller::checkDependencyList(std::string nameOfFile, DBProvider *dbProvider) {
	FileParser parser;
	if (parser.checkInputCorrect(nameOfFile)) {
		DBObjects objectsParameters = parser.getResultOfParsing(nameOfFile);
		DependenciesChecker checker;

		bool result = checker.getCheck(objectsParameters, dbProvider);
		passCheckLogsForGui(checker.getDataForLog(), result);
		checker.print();
		createLog("Temp/CheckingDependenciesErrors.log", INFO, checker.getDataForLog());

		return result;
	}
	else {
		throw std::invalid_argument("Wrong DependencyList.dpn file");
	}
}

/** Installation part. */

/** Inner methods. */
std::string readLogFromTempFile(std::string fileName) {
	std::string dataForLog;
	std::string buffer;
	std::ifstream temp(fileName, std::ios::in);

	while (getline(temp, buffer)) {
		dataForLog += buffer;
		dataForLog += "\n";
	}

	return dataForLog;
}

void passInstallLogsGui(std::string &dataForErrorLog, std::string &dataForInfoLog, bool installWithErrors) {
	std::cerr << "INSTALLATION PROCESS:\n";
	std::cerr << dataForInfoLog;
	std::cerr << "INSTALLATION ERRORS:\n";
	dataForErrorLog += "Installation completed";
	if (dataForErrorLog.empty()) {	
		dataForErrorLog += " without errors.\n";
	}
	else {
		dataForErrorLog += " with errors.\n";
	}
	std::cerr << dataForErrorLog;
}

/** Main method of installing part.
	When the method starts, the dependency check is considered successful. */
bool PatchInstaller::startInstallation() {
	system("Install.bat");

	std::ifstream errors("tempError.txt", std::ios::in);
	std::ifstream info("tempInfo.txt", std::ios::in);

	std::string dataForInfoLog = readLogFromTempFile("tempInfo.txt");
	std::string dataForErrorLog = readLogFromTempFile("tempError.txt");
	bool installWithErrors = dataForErrorLog.empty();
	passInstallLogsGui(dataForErrorLog, dataForInfoLog, installWithErrors);

	errors.close();
	info.close();
	remove("tempError.txt");
	remove("tempInfo.txt");

	createLog("Temp/InstallationErrors.log", ERROR, dataForErrorLog);
	createLog("Temp/InstallationInfo.log", INFO, dataForInfoLog);

	return installWithErrors;
}
