#ifndef LOGGER_H
#define LOGGER_H

//#include "PatchInstaller/PatchInstaller.h"



class Logger {
public:
	Logger();
	~Logger();
	std::string getCurrentDateTime();
	void addLog(std::string message, std::string logFileFullName);
};
#endif