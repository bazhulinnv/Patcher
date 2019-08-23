#ifndef DEPENDENCIESCHECKER_H
#define DEPENDENCIESCHECKER_H

#include <iostream>
#include <unordered_map>

#include "DBProvider/DBProvider.h"
#include "PatchInstaller.h"

class DependenciesChecker {
public:
  DependenciesChecker();
  ~DependenciesChecker();
  // getters
  std::list<bool> getListWithExistenceEachObject();
  bool getIfAllObjectsExists();
  std::string getDataForLog();

  // wrappers for private methods: check dependencies list of objects; print
  // existence of each object for gui
  bool getCheck(DBObjs &object_parameters, DBProvider &provider);
  void print();

private:
  std::list<bool> existence_each_object;
  bool all_objects_exists;
  std::string data_for_log;
  bool DependenciesChecker::check(DBObjs &objects_parameters,
                                  DBProvider &provider);
  void DependenciesChecker::printExistenceOfEachObject();
};
#endif