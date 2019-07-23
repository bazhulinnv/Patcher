#include <fstream>
#include <iostream>
#include <array>
#include <time.h>
#include <direct.h>

#include "PatchInstaller/PatchInstaller.h"
#include "PatchInstaller/DependenciesChecker.h"
#include "DBProvider/DBProviderLogger.h"

using namespace DBProviderLogger;

PatchInstaller::PatchInstaller() {}
PatchInstaller::~PatchInstaller() {}


/** The function checks the presence of objects in the database according to the list of objects specified in the file. */
bool PatchInstaller::checkObjectsForExistenceFromFile(std::string nameOfFile, DBProvider dbProvider) {
	FileParser fileParser;
	std::list<std::tuple<std::string, std::string, std::string>> objectsNameAndType = fileParser.parse(nameOfFile);
	DependenciesChecker checker;
	checker.check(checker, objectsNameAndType, dbProvider);
	checker.printExistenceOfEachObject(checker);
	std::cout << checker.dataForLog;
	//DBLog logger;
	//logger.addLog(INFO, checker.dataForLog);
	return checker.allObjectsExists;
}


/** When the method starts, the dependency check is considered successful. */
/** Figuring out the operating system. */
#if !defined(__WIN32__) && (defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__))
#  define __WIN32__
#endif

#if defined(__unix__) || defined(__unix) || defined(_UNIXWARE7)
#define __unix__
#endif

/** When the method starts, the dependency check is considered successful. */
bool PatchInstaller::startInstallation(std::string directory) {
	std::string command0("cd " + directory);
	std::string command1("Install.bat");
	//command("Install.bat");
	std::array<char, 128> buffer;
	std::string dataForLog;

#if defined(__WIN32__)
	FILE* pipe0 = _popen(command0.c_str(), "r");
	if (!pipe0) {
		std::cerr << "Couldn't start command." << std::endl;
		return 0;
	}
	while (fgets(buffer.data(), 128, pipe0) != NULL) {
		dataForLog += buffer.data();
	}
	std::cout << "!!!!!!" << dataForLog << "!!!\n";

	FILE* pipe1 = _popen(command1.c_str(), "r");
	if (!pipe1) {
		std::cerr << "Couldn't start command." << std::endl;
		return 0;
	}
	while (fgets(buffer.data(), 128, pipe1) != NULL) {
		dataForLog += buffer.data();
	}
	std::cout << "!!!!!!" << dataForLog << "!!!\n";
	//auto returnCode = _pclose(pipe0);
	//auto returnCode = _pclose(pipe1);
#endif

#if (defined(__unix__)) 
	FILE* pipe = popen(command.c_str(), "r");
	if (!pipe) {
		std::cerr << "Couldn't start command." << std::endl;
		return 0;
	}
	while (fgets(buffer.data(), 128, pipe) != NULL) {
		dataForLog += buffer.data();
	}
	auto returnCode = pclose(pipe);
#endif
	return true;
}
