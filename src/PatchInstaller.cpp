#include <fstream>
#include <iostream>
#include <array>
#include <time.h>
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
	std::array<char, 128> buffer;
	std::string result;

#if (defined(__WIN32__))
	FILE* pipe = _popen("Install.bat", "r");
	if (!pipe)
	{
		std::cerr << "Couldn't start command." << std::endl;
		return 0;
	}
	while (fgets(buffer.data(), 128, pipe) != NULL) {
		result += buffer.data();
	}
	auto returnCode = _pclose(pipe);

	std::cout << "!!!" << result << "!!" << std::endl;
	//std::cout << returnCode << std::endl;
#endif
#if (defined(__unix__)) 
	FILE* pipe = popen("Install.bat", "r");
	if (!pipe)
	{
		std::cerr << "Couldn't start command." << std::endl;
		return 0;
	}
	while (fgets(buffer.data(), 128, pipe) != NULL) {
		result += buffer.data();
	}
	auto returnCode = pclose(pipe);

	std::cout << "!!!" << result << "!!" << std::endl;
	//std::cout << returnCode << std::endl;
#endif
}
