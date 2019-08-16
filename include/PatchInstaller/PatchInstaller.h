#ifndef PATCHINSTALLER_H
#define PATCHINSTALLER_H

#include <cstdio>
#include <string>
#include "DBProvider/DBProvider.h"
#include "DBProvider/LoginData.h"

//type for object identification: schema-name-type
using DBObject = std::tuple<std::string, std::string, std::string>;
//list of objects
using DBObjects = std::list<DBObject>;

class PatchInstaller {
public:
  PatchInstaller();
  ~PatchInstaller();
  //reading list of objects from DependencyList.dpn and check it for existence
  bool checkDependencyList(std::string nameOfFile, DBProvider &dbProvider);
  //start installation script
  bool startInstallation(LoginData p);
};

#endif