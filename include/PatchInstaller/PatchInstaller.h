#ifndef PATCHINSTALLER_H
#define PATCHINSTALLER_H

#include "DBProvider/DBProvider.h"
#include "DBProvider/LoginData.h"
#include <string>

using namespace Provider;

// type for object identification: schema-name-type
using DBObj = std::tuple<std::string, std::string, std::string>;
// list of objects
using DBObjs = std::list<DBObj>;

using namespace Provider;

class PatchInstaller {
public:
  PatchInstaller(const std::string& conn_param);
  
  ~PatchInstaller();
  
  // reading list of objects from DependencyList.dpn and check it for existence
  bool checkDependencyList(const std::string& file_name);
  
  // start installation script
  bool startInstallation(LoginData parameters) const;

private:
  DBProvider provider;
};

#endif