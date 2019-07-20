#include <DBProvider/DBProviderLogger.h>
#include <exception>

namespace DBProviderLogger
{
	DBLog::DBLog()
	{
	}

	// Create our global logging object.
	DBLog::DBLog(const std::string &filepath) : myLog{}
	{
		myLog.open(filepath);
	}

	DBLog::~DBLog()
	{
		log(INFO, "Stopped logging system.");
		myLog.close();
	}

	// Add a message to the log.
	void DBLog::addLog(Level s, const std::string &msg)
	{
		if (myLog.is_open())
		{
			myLog << levels[static_cast<int>(s)] << ": " << msg << std::endl;
			return;
		}
		
		throw new std::exception("ERROR: Log file currupted or missing.\n");
	}

	void DBLog::setPathAndOpen(const std::string &filepath)
	{
		myLog.open(filepath);

		if (!myLog.is_open())
		{
			throw new std::exception("ERROR: Failed opening log file.\n");
		}
	}
	
	// Global Logging Object.
	inline DBLog* getGlobalLog()
	{
		static std::unique_ptr<DBLog> glog = std::make_unique<DBLog>();
		return glog.get();
	}

	// Initalize our logging object.
	void startLog(const std::string &filepath)
	{
		DBLog *glog = getGlobalLog();
		glog->setPathAndOpen(filepath);
		log(INFO, "Started logging system.");
	}

	// Method which logs.
	void log(Level s, const std::string &msg)
	{
		DBLog* glog = getGlobalLog();
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