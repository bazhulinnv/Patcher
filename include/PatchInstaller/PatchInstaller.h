#ifndef PATCHINSTALLER_H
#define PATCHINSTALLER_H
#include <cstdio>

class PatchInstaller {
public:
  PatchInstaller();
  ~PatchInstaller();
  bool checkObjectsForExistence(FILE* file);
};
#endif