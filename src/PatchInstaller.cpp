#include <cstdio>
#include <String>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <array>
#include <list>
#include <time.h>
#include <direct.h>

#include "PatchInstaller/PatchInstaller.h"
#include "DBProvider/DBProvider.h"
#include "PatchInstaller/PatchInstaller.h"

using namespace std;

constexpr auto LOG_NAME = "PatchBuilder_"; // Begining of the log file name
constexpr auto LOG_FOLDER = "Logs"; // Folder name for logs
constexpr auto LOG_FORMAT = ".log"; // Folder name for logs

PatchInstaller::PatchInstaller() {}
PatchInstaller::~PatchInstaller() {}

std::string PatchInstaller::getCurrentDateTime() {
	// Getting current date
	time_t now;
	time(&now);
	struct tm  tstruct;
	char  buf[80];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%F-%H-%M-%S", &tstruct); // Returning time in "year-month-day-hour-minute-second" format
	return string(buf);
}

void PatchInstaller::passParametersToDBProvider(char *parameters, DBProvider dbProvider) {
	dbProvider.connect(parameters);
}

void PatchInstaller::addLog(std::string message, std::string logFileFullName)
{
	// Writing message in log file
	ofstream output(logFileFullName, std::ios_base::app);
	output << message;
	output.close();
}	


/** The function checks the presence of objects in the database according to the list of objects specified in the file. */
bool PatchInstaller::checkObjectsForExistence(std::string nameOfFile, DBProvider dbProvider)
{
	string logDirectory = "CheckingDependenciesLogs";
	mkdir(&logDirectory[0]);

	bool result = true;

	ifstream dependencies(nameOfFile, ios::in);

	std::string buffer("");
	std::string objectName("");
	std::string objectType("");
	list <bool> objectsExistenceForGUI;
	std::string resultOfLogging;

	//Try to read first string from file
	dependencies >> objectName >> objectType;

	if ((objectName != "") && (objectType != "")) {
		while (getline(dependencies, buffer)) {
			
			result = result && dbProvider.isCurrentObjectExist(objectName, objectType);
			resultOfLogging += objectName + " " + objectType + " ";
			if (!dbProvider.isCurrentObjectExist(objectName, objectType)) {
				resultOfLogging += "not ";
			}
	
			resultOfLogging  += "exists\n";
			objectsExistenceForGUI.push_back(dbProvider.isCurrentObjectExist(objectName, objectType));
			dependencies >> objectName >> objectType;
		}

		resultOfLogging += objectName + " " + objectType + " ";
		if (!dbProvider.isCurrentObjectExist(objectName, objectType)) {
			resultOfLogging += "not ";
		}

		resultOfLogging += "exists\n";
		std::string logFileFullName = logDirectory + "//" + LOG_NAME + getCurrentDateTime() + LOG_FORMAT;
		addLog(resultOfLogging, logFileFullName);

		objectsExistenceForGUI.push_back(dbProvider.isCurrentObjectExist(objectName, objectType));

		copy(objectsExistenceForGUI.begin(), objectsExistenceForGUI.end(), ostream_iterator<int>(cout, ""));
	}

	dependencies.close();

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
	string logDirectory = "InstallingLogs";
	mkdir(&logDirectory[0]);
	string logFileFullName = logDirectory + "//" + LOG_NAME + getCurrentDateTime() + LOG_FORMAT;
	addLog(result, logFileFullName);
	return true;
}

