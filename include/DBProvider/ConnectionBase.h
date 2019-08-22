#ifndef DBCONNECTIONBASE_H
#define DBCONNECTIONBASE_H

#include "DBProvider/LoginData.h"
#include <pqxx/pqxx>
#include <string>

namespace DBConnection {
class ConnectionBase {
public:
  ConnectionBase();

  virtual ~ConnectionBase();

  virtual void SetConnection(std::string &parameters) = 0;

  virtual bool IsConnectionSet();

  virtual bool IsOpen();

  virtual LoginData GetParameters() = 0;

  virtual std::shared_ptr<pqxx::connection_base> GetConnection() = 0;

protected:
  bool parameters_set_ = false;
  bool connection_set_ = false;
  LoginData connection_params_;
  std::shared_ptr<pqxx::connection_base> db_connection_;

private:
  virtual void CloseConnection() = 0;
};
} // namespace DBConnection

#endif