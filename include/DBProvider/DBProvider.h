#ifndef DBPROVIDER_H
#define DBPROVIDER_H
#include <string>

class DBProvider {
public:
  DBProvider();
  ~DBProvider();
  int getObjects();
  void connect(char *parameters);
  bool isCurrentObjectExist(std::string scheme, std::string objectName, std::string objectType);
};
#endif