#include <fstream>
#include <iostream>
#include <array>
#include <time.h>
#include <direct.h>

#include "PatchInstaller/PatchInstaller.h"
#include "PatchInstaller/DependenciesChecker.h"

PatchInstaller::PatchInstaller() {}
PatchInstaller::~PatchInstaller() {}

/** Logging part (requires a separate class).*/

constexpr auto LOG_NAME = "PatchInstaller_"; // Begining of the log file name
constexpr auto LOG_FOLDER = "Logs"; // Folder name for logs
constexpr auto LOG_FORMAT = ".log"; // Folder name for logs

std::string getCurrentDateTime() {
	// Getting current date
	time_t now;
	time(&now);
	struct tm  tstruct;
	char  buf[80];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%F-%H-%M-%S", &tstruct); // Returning time in "year-month-day-hour-minute-second" format
	return std::string(buf);
}

void PatchInstaller::passParametersToDBProvider(char *parameters, DBProvider dbProvider) {
	dbProvider.connect(parameters);
}

void PatchInstaller::addLog(std::string message, std::string logFileFullName)
{
	// Writing message in log file
	std::ofstream output(logFileFullName, std::ios_base::app);
	output << message;
	output.close();
}
// End of logging part.


/** The function checks the presence of objects in the database according to the list of objects specified in the file. */
bool PatchInstaller::checkObjectsForExistenceFromFile(std::string nameOfFile, DBProvider dbProvider) {
	//needs logs!
	FileParser fileParser;
	std::list<std::tuple<std::string, std::string, std::string>> objectsNameAndType = fileParser.parse(nameOfFile);
	//std::cout << "In checkObjectsForExistenceFromFile:\n";
	//std::cout << objectsNameAndType.size() << "<- size in method higher\n";
	DependenciesChecker checker;
	checker.check(checker, objectsNameAndType, dbProvider);
	checker.printExistenceOfEachObject(checker);

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
	std::string logDirectory = "InstallingLogs";
	mkdir(&logDirectory[0]);
	std::string logFileFullName = logDirectory + "//" + LOG_NAME + getCurrentDateTime() + LOG_FORMAT;
	Logger logger;
	logger.addLog(result, logFileFullName);
	return true;
}
