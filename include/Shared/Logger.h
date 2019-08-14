#include <memory>
#include <fstream>
#include <string>

namespace PatcherLogger
{
	// Severity level enum.
	enum Level
	{
		DEBUG,
		INFO,
		WARNING,
		ERROR,
		FATAL,
	};
	
	// Logging class.
	class Log
	{
	public:

		// Creates empty instance.
		Log();
		
		// Creates log file on specified path.
		Log(const std::string &filePath);		
		
		// Safely deletes log.
		~Log();

		// Sets log file by path.
		void setLogByPath(const std::string& filePath);
		
		// Sets log file by name using stdLoggingPath.
		void setLogByName(const std::string& logName);
		
		// Returns current log path
		std::string getCurrentLogPath();

		// Returns standard logging directory
		std::string getStdLoggingPath();

		// Resets standard logging directory
		void setStdLoggingPath(const std::string& path);

		// Writes to current log file
		void addLog(Level s, const std::string &msg);

		// Closes current log
		void closeLog();

	private:
		// File for logging.
		std::ofstream _currentLog;

		// Path to log.
		std::string _logPath;

		// Path to standard log directory.
		std::string _stdLoggingPath = "../build/DBProvider.dir/";
		
		// Message levels
		std::string _levels[5] = { "Debug", "Info", "Warning", "Error", "Fatal" };

		/*	Opens log if path is correct,
			otherwise throws exception.
		*/
		void tryToOpenLog(const std::string& filePath);
	};

	// Initalizes global log.
	void startGlobalLog();
	
	// Initalizes global log on peth.
	void startGlobalLog(const std::string& filePath);

	// Stops global log.
	void stopGlobalLog();

	// Logs a message to global log.
	void log(Level s, const std::string &msg);

	// Logs message with level:FATAL to global log.
	void logFatal(const std::string &msg);

	// Logs message with level:ERROR to global log.
	void logError(const std::string &msg);

	// Logs message with level:WARNING to global log.
	void logWarning(const std::string &msg);

	// Logs message with level:INFO to global log.
	void logInfo(const std::string &msg);

	// Logs message with level:DEBUG to global log.
	void logDebug(const std::string &msg);
}