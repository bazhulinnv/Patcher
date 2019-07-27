#include "Shared/Logger.h"

#include <exception>
#include <fstream>
#include <iostream>
#include <ctime>

namespace PatcherLogger
{	
	// Returns current date-time formatted like [YYYY-MM-DD] [HH:mm:ss]
	inline const std::string GetCurrentDateTime()
	{
		time_t currentTime = time(0);
		struct tm localDateTime;
		char buf[80];

		localtime_s(&localDateTime, &currentTime);
		strftime(buf, sizeof(buf), "[%Y-%m-%d] [%X]", &localDateTime);

		return buf;
	}

	Log::Log()
	{
	}

	Log::Log(const std::string &filePath) : _currentLog{}
	{
		tryToOpenLog(filePath);
		_logPath = filePath;
	}

	Log::~Log()
	{
		closeLog();
	}

	void Log::addLog(Level s, const std::string &msg)
	{
		if (!_currentLog.is_open())
		{
			std::cerr	<<	"\nFirst you need to set up custom log path\nExample:\n\t"
							"auto customLog = new DBLog();\n\t"
							"customLog->setPathAndOpen(\"../pat_to_log/customLog.log\");"
						<< std::endl;
			throw new std::exception("ERROR: Log file was not set or missing.\n");
		}

		_currentLog << GetCurrentDateTime() << " ";
		_currentLog << _levels[static_cast<int>(s)] << ": " << msg << std::endl;
	}

	void Log::setLogByPath(const std::string &filePath)
	{
		tryToOpenLog(filePath);
		_logPath = filePath;
	}

	void Log::setLogByName(const std::string &logName)
	{
		tryToOpenLog(_stdLoggingPath + logName);
		_logPath = _stdLoggingPath + logName;
	}

	// Returns current log path
	std::string Log::getCurrentLogPath()
	{
		return _logPath;
	}

	// Returns standard logging directory
	std::string Log::getStdLoggingPath()
	{
		return _stdLoggingPath;
	}

	// Resets standard logging directory
	void Log::setStdLoggingPath(const std::string& path)
	{
		_stdLoggingPath = path;
	}

	void Log::tryToOpenLog(const std::string& filePath)
	{
		// handle ofstream properly
		const char *filename = filePath.c_str();
		_currentLog.open(filename, std::ios::app);

		if (!_currentLog.is_open())
		{
			std::cerr << "Logger: incorrect path to log." << std::endl;
			throw new std::exception("ERROR: Failed opening log file.\n");
		}
	}

	// Closes current log
	inline void Log::closeLog()
	{
		_currentLog.close();
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