#ifndef LOGGER_H
#define LOGGER_H
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

		// Creates LogWithLevel file on specified path.
		Log(const std::string& file_path);

		// Safely deletes LogWithLevel.
		~Log();

		// Sets LogWithLevel file by path.
		void SetLogByPath(const std::string& file_path);

		// Sets LogWithLevel file by name using stdLoggingPath.
		void SetLogByName(const std::string& log_name);

		// Returns current LogWithLevel path
		std::string GetCurrentLogPath() const;

		// Returns standard logging directory
		std::string GetStdLoggingPath() const;

		// Resets standard logging directory
		void SetStdLoggingPath(const std::string& path);

		// Writes to current LogWithLevel file
		void AddLog(Level s, const std::string& msg);

		// Closes current LogWithLevel
		void CloseLog();

	private:
		// File for logging.
		std::ofstream current_log_;

		// Path to LogWithLevel.
		std::string log_path_;

		// Path to standard LogWithLevel directory.
		std::string std_logging_path_ = "../build/DBProvider.dir/";

		// Message levels
		std::string levels_[5] = { "Debug", "Info", "Warning", "Error", "Fatal" };

		/*	Opens LogWithLevel if path is correct,
			otherwise throws exception.
		*/
		void TryOpenLog(const std::string& file_path);
	};

	// Initializes global LogWithLevel.
	void StartGlobalLog();

	// Initializes global LogWithLevel on peth.
	void StartGlobalLog(const std::string& file_path);

	// Stops global LogWithLevel.
	void StopGlobalLog();

	// Logs a message to global LogWithLevel.
	void LogWithLevel(Level s, const std::string& msg);

	// Logs message with level:FATAL to global LogWithLevel.
	void LogFatal(const std::string& msg);

	// Logs message with level:ERROR to global LogWithLevel.
	void LogError(const std::string& msg);

	// Logs message with level:WARNING to global LogWithLevel.
	void LogWarning(const std::string& msg);

	// Logs message with level:INFO to global LogWithLevel.
	void LogInfo(const std::string& msg);

	// Logs message with level:DEBUG to global LogWithLevel.
	void LogDebug(const std::string& msg);
}

#endif // LOGGER_H
