#include "PatchInstaller/PatchInstaller.h"
#include "PatchInstaller/DependenciesChecker.h"
#include "PatchInstaller/FileParser.h"
#include "Shared/Logger.h"

#include <filesystem>
#include <fstream>
#include <iostream>

using namespace PatcherLogger;

PatchInstaller::PatchInstaller(std::string conn_params) {
  provider = DBProvider(conn_params);
}
PatchInstaller::~PatchInstaller() {}

/** Creates log for both parts of PatchInstaller. */
void createLog(std::string path, Level level, std::string data) {
  Log log;
  log.SetLogByPath(path);
  log.AddLog(level, data);
}

/** Checking dependencies part. */
void passCheckLogsForGui(std::string data, bool result_check) {
  std::cerr << "DEPENDENCY CHECK PROCESS:\n";
  std::cerr << data;
  if (!result_check) {
    std::cerr
        << "Check completed. Some objects do not exist in current database.\n";
  } else {
    std::cerr << "Check completed. All objects exist in current database.\n";
  }
}

/** Main method of checking dependencies.
        Checks the presence of objects in the database according to the list of
   objects specified in the file. */
bool PatchInstaller::checkDependencyList(std::string file_name) {
  FileParser parser;
  if (parser.checkInputCorrect(file_name)) {
    // A list of objects with three parameters: schema-name-type is created from
    // the file
    DBObjs objects_parameters = parser.getResultOfParsing(file_name);
    DependenciesChecker checker;

    // result = true if all object exist in current database
    bool result = checker.getCheck(objects_parameters, provider);
    // print in stdout list of existence objects: schema name type exists /
    // schema name type does not exist
    passCheckLogsForGui(checker.getDataForLog(), result);
    // print in stdout the same list of existence objects: 0 if object does not
    // exist, 1 if object exists
    checker.print();
    createLog("logs/CheckingDependenciesErrors.log", INFO,
              checker.getDataForLog());

    return result;
  } else {
    throw std::invalid_argument("Incorrect DependencyList.dpn file");
  }
}

/** Installation part. */

/** Inner methods. */
std::string readLogFromTempFile(std::string file_name) {
  std::string data_for_log;
  std::string buffer;
  std::ifstream temp(file_name, std::ios::in);

  while (getline(temp, buffer)) {
    data_for_log += buffer;
    data_for_log += "\n";
  }

  return data_for_log;
}

void passInstallLogsGui(std::string &data_for_error_log,
                        bool install_with_errors) {
  std::cerr << "INSTALLATION ERRORS:\n";
  if (data_for_error_log.empty()) {
    data_for_error_log += "Installation completed.\n";
  } else {
    data_for_error_log += "Installation completed WITH ERRORS.\n";
  }
  std::cerr << data_for_error_log;
}

/** Figuring out the operating system. */
#if !defined(__WIN32__) &&                                                     \
    (defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__))
#define __WIN32__
#endif

#if defined(__unix__) || defined(__unix) || defined(_UNIXWARE7)
#define __unix__
#endif

/** Main method of installing part.
        When the method starts, the dependency check is considered successful.
 */
bool PatchInstaller::startInstallation(LoginData p) {
  // build command with parameters: Install.bat username databaseName host port
#if defined(__WIN32__)
  std::string command_with_connection =
      "Install.bat " + p.username + +" " + p.database + " " + p.hostname + " ";
  command_with_connection += std::to_string(p.port);
#endif
#if (defined(__unix__))
  std::string commandWithParametersConnection =
      "Install.sh " + p.username + +" " + p.database + " " + p.hostname + " ";
  commandWithParametersConnection += std::to_string(p.port);
#endif
  const char *cstr = command_with_connection.c_str();
  // start installation script with parameters
  system(cstr);

  // open temp file generated by installation script
  std::ifstream errors("tempError.txt", std::ios::in);
  std::ifstream info("tempInfo.txt", std::ios::in);
  try {
    std::string data_for_info_log = readLogFromTempFile("tempInfo.txt");
    std::string data_for_error_log = readLogFromTempFile("tempError.txt");
    bool install_with_errors = data_for_error_log.empty();
    // print in stdout installation errors
    passInstallLogsGui(data_for_error_log, install_with_errors);

    errors.close();
    info.close();
    remove("tempError.txt");
    remove("tempInfo.txt");

    createLog("logs/InstallationErrors.log", ERROR, data_for_error_log);
    createLog("logs/InstallationInfo.log", INFO, data_for_info_log);
    return install_with_errors;
  } catch (std::exception &ex) {
    errors.close();
    info.close();
    return false;
  }
  // return installWithErrors;
}
