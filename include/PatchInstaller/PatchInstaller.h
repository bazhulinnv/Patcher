#ifndef PATCHINSTALLER_H
#define PATCHINSTALLER_H

#include "DBProvider/DBProvider.h"
#include "DBProvider/LoginData.h"
#include <string>

// type for object identification: schema-name-type
using DBObj = std::tuple<std::string, std::string, std::string>;
// list of objects
using DBObjs = std::list<DBObj>;

using namespace Provider;

class PatchInstaller {
public:
  PatchInstaller(std::string conn_param);
  
  ~PatchInstaller();
  
  // reading list of objects from DependencyList.dpn and check it for existence
  bool checkDependencyList(std::string file_name);
  
  // start installation script
  bool startInstallation(LoginData p);

private:
  DBProvider provider;
};

#endif