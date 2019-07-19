#include <time.h>
#include <direct.h>
#include <fstream>
#include <string>

#include "PatchInstaller/Logger.h"

Logger::Logger() {};
Logger::~Logger() {};

constexpr auto LOG_NAME = "PatchInstaller_"; // Begining of the log file name
constexpr auto LOG_FOLDER = "Logs"; // Folder name for logs
constexpr auto LOG_FORMAT = ".log"; // Folder name for logs

std::string Logger::getCurrentDateTime() {
	// Getting current date
	time_t now;
	time(&now);
	struct tm  tstruct;
	char  buf[80];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%F-%H-%M-%S", &tstruct); // Returning time in "year-month-day-hour-minute-second" format
	return std::string(buf);
}

void Logger::addLog(std::string message, std::string logFileFullName)
{
	// Writing message in log file
	std::ofstream output(logFileFullName, std::ios_base::app);
	output << message;
	output.close();
}