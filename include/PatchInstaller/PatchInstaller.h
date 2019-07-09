#ifndef PATCHINSTALLER_H
#define PATCHINSTALLER_H

#include <cstdio>
#include <string>

class PatchInstaller {
public:
  PatchInstaller();
  ~PatchInstaller();
  bool checkObjectsForExistence(std::string nameOfFile);
  bool startInstallation();
};
#endif