#ifndef DBPROVIDER_H
#define DBPROVIDER_H
#include <string>

class DBProvider {
public:
  DBProvider();
  ~DBProvider();
  int getObjects();
  void connect();
  bool isCurrentObjectExist(std::string objectName, std::string objectType);
};
#endif