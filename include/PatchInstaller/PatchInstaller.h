#ifndef PATCHINSTALLER_H
#define PATCHINSTALLER_H

#include <cstdio>
#include <string>
#include "DBProvider/DBProvider.h"
#include "FileParser.h"

class PatchInstaller {
public:
  PatchInstaller();
  ~PatchInstaller();
  bool checkObjectsForExistenceFromFile(std::string nameOfFile, DBProvider dbProvider);
  bool startInstallation(char* directory);
};

#endif