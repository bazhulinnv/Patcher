#include "Shared/Logger.h"

#include <exception>
#include <fstream>
#include <iostream>
#include <ctime>

namespace PatcherLogger
{	
	// Returns current date-time formatted like [YYYY-MM-DD][HH:mm:ss]
	inline const std::string GetCurrentDateTime()
	{
		time_t currentTime = time(0);
		struct tm localDateTime;
		char buf[80];

		localtime_s(&localDateTime, &currentTime);
		strftime(buf, sizeof(buf), "[%Y-%m-%d][%X]", &localDateTime);

		return buf;
	}

	Log::Log()
	{
	}

	Log::Log(const std::string &filePath) : currentLog{}
	{
		tryOpenLog(filePath);
		logPath = filePath;
	}

	Log::~Log()
	{
		closeLog();
	}

	void Log::addLog(Level s, const std::string &msg)
	{
		if (!currentLog.is_open())
		{
			std::cerr	<<	"\nFirst you need to set up custom log path\nExample:\n\t"
							"auto customLog = new DBLog();\n\t"
							"customLog->setPathAndOpen(\"../pat_to_log/customLog.log\");"
						<< std::endl;
			throw new std::exception("ERROR: Log file was not set or missing.\n");
		}

		currentLog << GetCurrentDateTime() << " ";
		currentLog << levels[static_cast<int>(s)] << ": " << msg << std::endl;
	}

	// Closes current log

	inline void Log::closeLog()
	{
		currentLog.close();
	}

	void Log::setLogByPath(const std::string &filePath)
	{
		tryOpenLog(filePath);
		logPath = filePath;
	}

	void Log::setLogByName(const std::string& logName)
	{
		tryOpenLog(stdLoggingPath + logName);
		logPath = stdLoggingPath + logName;
	}

	// Returns current log path
	inline std::string Log::getCurrentLogPath()
	{
		return logPath;
	}

	// Returns standard logging directory
	inline std::string Log::getStdLoggingPath()
	{
		return stdLoggingPath;
	}

	// Resets standard logging directory
	inline void Log::setStdLoggingPath(const std::string& path)
	{
		stdLoggingPath = path;
	}

	inline void Log::tryOpenLog(const std::string& filePath)
	{
		// handle ofstream properly
		const char *filename = filePath.c_str();
		currentLog.open(filename, std::ios::app);

		if (!currentLog.is_open())
		{
			std::cerr << "Logger: incorrect path to log." << std::endl;
			throw new std::exception("ERROR: Failed opening log file.\n");
		}
	}

	// Global Log Object.
	inline Log *getGlobalLog()
	{
		static std::unique_ptr<Log> glog = std::make_unique<Log>();
		return glog.get();
	}

	void startGlobalLog()
	{
		Log *glog = getGlobalLog();
		const std::string path = glog->getStdLoggingPath() + "global_log.log";
		glog->setLogByPath(path);
		log(INFO, "Started global logging.");
	}

	void startGlobalLog(const std::string &filePath)
	{
		Log *glog = getGlobalLog();
		glog->setLogByPath(filePath);
		log(INFO, "Started global logging.");
	}

	void stopGlobalLog()
	{
		Log *glog = getGlobalLog();
		log(INFO, "Stopped global logging.");
		glog->closeLog();
	}

	void log(Level s, const std::string &msg)
	{
		Log *glog = getGlobalLog();
		glog->addLog(s, msg);
	}

	void logFatal(const std::string &msg)
	{
		log(FATAL, msg);
	}
	
	void logError(const std::string &msg)
	{
		log(ERROR, msg);
	}
	
	void logWarning(const std::string &msg)
	{
		log(WARNING, msg);
	}
	
	void logInfo(const std::string &msg)
	{
		log(INFO, msg);
	}

	void logDebug(const std::string &msg)
	{
		log(DEBUG, msg);
	}
}