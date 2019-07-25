#include <fstream>
#include <iostream>
#include <array>
#include <time.h>
#include <filesystem>
#include <direct.h>

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
	checker.printExistenceOfEachObject();
	return result;
}


/** When the method starts, the dependency check is considered successful. */

// You can use a simple guard class to make sure the buffer is always reset:
struct cerrRedirect
{
	cerrRedirect(std::streambuf *new_buffer) : old(std::cerr.rdbuf(new_buffer)) {}
	~cerrRedirect() { std::cerr.rdbuf(old); }
private:
	std::streambuf *old;
};

struct cout_redirect {
	cout_redirect(std::streambuf * new_buffer)
		: old(std::cout.rdbuf(new_buffer))
	{ }

	~cout_redirect() {
		std::cout.rdbuf(old);
	}

private:
	std::streambuf * old;
};

	/** When the method starts, the dependency check is considered successful. */
/** Figuring out the operating system. */
#if !defined(__WIN32__) && (defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__))
#  define __WIN32__
#endif

#if defined(__unix__) || defined(__unix) || defined(_UNIXWARE7)
#define __unix__
#endif

/** When the method starts, the dependency check is considered successful. */
bool PatchInstaller::startInstallation(char* directory) {
	chdir(directory);
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

	std::ofstream logError("logError.log", ios::out | ios::app);
	logError << dataForErrorLog;
	std::ofstream logInfo("logInfo.log", ios::out | ios::app);
	logInfo << dataForInfoLog;
	if (dataForErrorLog == "") {
		resultOfInstall = true;
	}
	logError.close();
	logInfo.close();
	errors.close();
	info.close();
	return resultOfInstall;
}
