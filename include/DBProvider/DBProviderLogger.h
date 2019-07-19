#pragma once
#include <memory>
#include <fstream>
#include <string>

namespace DBProviderLogger
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
	class DBLog
	{
	public:
		DBLog();
		DBLog(const std::string &filepath);		
		~DBLog();

		void setPathAndOpen(const std::string& filepath);
		void addLog(Level s, const std::string &msg);

	private:
		// File for logging.
		std::ofstream myLog;
		std::string levels[5] = { "Debug", "Info", "Warning", "Error", "Fatal" };
	};

	// Initialize the log.
	void startLog(const std::string &filepath);

	// Log a message.
	void log(Level s, const std::string &msg);

	// Logs a message with specified level.
	void logFatal(const std::string &msg);
	void logError(const std::string &msg);
	void logWarning(const std::string &msg);
	void logInfo(const std::string &msg);
	void logDebug(const std::string &msg);
}