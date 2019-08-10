#ifndef PATCHINSTALLER_H
#define PATCHINSTALLER_H

#include <cstdio>
#include <string>
#include "DBProvider/DBProvider.h"

using DBObject = std::tuple<std::string, std::string, std::string>;
using DBObjects = std::list<DBObject>;

class PatchInstaller {
public:
  PatchInstaller();
  ~PatchInstaller();
  bool checkDependencyList(std::string nameOfFile, DBProvider *dbProvider);
  bool startInstallation(std::pair<std::vector<std::string>, std::string> separateParameters);
};

#endif