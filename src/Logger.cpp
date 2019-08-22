#include "Shared/Logger.h"
#include <ctime>
#include <exception>
#include <fstream>
#include <iostream>
#include <memory>

using namespace std;

namespace PatcherLogger {
// Returns current date-time formatted like [YYYY-MM-DD] [HH:mm:ss]
inline string GetCurrentDateTime() {
  auto current_time = time(nullptr);
  struct tm local_date_time;
  char buf[80];
  localtime_s(&local_date_time, &current_time);
  strftime(buf, sizeof(buf), "[%Y-%m-%d] [%X]", &local_date_time);
  return buf;
}

Log::Log() = default;

Log::Log(const string &file_path) : current_log_{} {
  TryOpenLog(file_path);
  log_path_ = file_path;
}

Log::~Log() { CloseLog(); }

void Log::AddLog(Level s, const string &msg) {
  if (!current_log_.is_open()) {
    cerr << "\nFirst you need to set up custom LogWithLevel path\nExample:\n\t"
            "auto custom_log = new DBLog();\n\t"
            "custom_log->setPathAndOpen(\"../pat_to_log/"
            "custom_log.LogWithLevel\");"
         << endl;
    throw exception("ERROR: Log file was not set or missing.\n");
  }

  current_log_ << GetCurrentDateTime() << " ";
  current_log_ << levels_[static_cast<int>(s)] << ": " << msg << endl;
}

void Log::SetLogByPath(const string &file_path) {
  TryOpenLog(file_path);
  log_path_ = file_path;
}

void Log::SetLogByName(const string &log_name) {
  TryOpenLog(std_logging_path_ + log_name);
  log_path_ = std_logging_path_ + log_name;
}

// Returns current LogWithLevel path
string Log::GetCurrentLogPath() const { return log_path_; }

// Returns standard logging directory
string Log::GetStdLoggingPath() const { return std_logging_path_; }

// Resets standard logging directory
void Log::SetStdLoggingPath(const string &path) { std_logging_path_ = path; }

void Log::TryOpenLog(const string &file_path) {
  // handle ofstream properly
  const char *filename = file_path.c_str();
  current_log_.open(filename, ios::app);

  if (!current_log_.is_open()) {
    cerr << "Logger: incorrect path to LogWithLevel." << endl;
    throw exception("ERROR: Failed opening LogWithLevel file.\n");
  }
}

// Closes current LogWithLevel
inline void Log::CloseLog() { current_log_.close(); }

// Global Log Object.
inline Log *getGlobalLog() {
  static unique_ptr<Log> glog = make_unique<Log>();
  return glog.get();
}

void StartGlobalLog() {
  Log *glog = getGlobalLog();
  const string path = glog->GetStdLoggingPath() + "global_log.LogWithLevel";
  glog->SetLogByPath(path);
  LogWithLevel(INFO, "Started global logging.");
}

void StartGlobalLog(const string &file_path) {
  Log *glog = getGlobalLog();
  glog->SetLogByPath(file_path);
  LogWithLevel(INFO, "Started global logging.");
}

void StopGlobalLog() {
  Log *glog = getGlobalLog();
  LogWithLevel(INFO, "Stopped global logging.");
  glog->CloseLog();
}

void LogWithLevel(Level s, const string &msg) {
  Log *glog = getGlobalLog();
  glog->AddLog(s, msg);
}

void LogFatal(const string &msg) { LogWithLevel(FATAL, msg); }

void LogError(const string &msg) { LogWithLevel(ERROR, msg); }

void LogWarning(const string &msg) { LogWithLevel(WARNING, msg); }

void LogInfo(const string &msg) { LogWithLevel(INFO, msg); }

void LogDebug(const string &msg) { LogWithLevel(DEBUG, msg); }
} // namespace PatcherLogger