#ifndef PATCHINSTALLER_H
#define PATCHINSTALLER_H

#include <cstdio>
#include <string>
#include "DBProvider/DBProvider.h"
#include "FileParser.h"
#include "Logger.h"

class PatchInstaller {
public:
  PatchInstaller();
  ~PatchInstaller();
  bool checkObjectsForExistenceFromFile(std::string nameOfFile, DBProvider dbProvider);
  bool startInstallation();
  void passParametersToDBProvider(char * parameters, DBProvider dbProvider);
  void addLog(std::string message, std::string logFileFullName);
};
#endif