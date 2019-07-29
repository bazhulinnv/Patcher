#ifndef PATCHINSTALLER_H
#define PATCHINSTALLER_H

#include <cstdio>
#include <string>
#include "DBProvider/DBProvider.h"

using DBProviderListParameters = std::list<std::tuple<std::string, std::string, std::string>>;

class PatchInstaller {
public:
  PatchInstaller();
  ~PatchInstaller();
  bool checkObjectsForExistenceFromFile(std::string nameOfFile, DBProvider *dbProvider);
  bool startInstallation();
};

#endif