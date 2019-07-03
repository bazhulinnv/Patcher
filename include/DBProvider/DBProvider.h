#ifndef DBPROVIDER_H
#define DBPROVIDER_H

class DBProvider {
public:
  DBProvider();
  ~DBProvider();
  int getObjects();
  void connect();
};
#endif