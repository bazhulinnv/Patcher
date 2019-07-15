#ifndef PATCHINSTALLER_H
#define PATCHINSTALLER_H

#include <cstdio>
#include <string>
#include "DBProvider/DBProvider.h"

class PatchInstaller {
public:
  PatchInstaller();
  ~PatchInstaller();
  void passParametersToDBProvider(char *parameters, DBProvider dbProvider);
  void addLog(std::string message, std::string logFileFullName);
  bool checkObjectsForExistence(std::string nameOfFile, DBProvider dbProvider);
  bool startInstallation();
  std::string getCurrentDateTime();
};
#endif