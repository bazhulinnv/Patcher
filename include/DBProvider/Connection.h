#ifndef CONNECTION_H
#define CONNECTION_H

#include "DBProvider/ConnectionBase.h"
#include "DBProvider/LoginData.h"
#include <memory>
#include <pqxx/pqxx>
#include <string>

namespace DBConnection {
class Connection : ConnectionBase {
public:
  Connection();

  explicit Connection(std::string &pgpass_str);

  bool IsConnectionSet() override;

  bool IsOpen() override;

  ~Connection();

  void SetConnection(std::string &pgpass_str) override;

  void SetConnection();

  LoginData GetParameters() override;

  std::shared_ptr<pqxx::connection_base> GetConnection() override;

private:
  void CloseConnection() override;
};
} // namespace DBConnection

#endif
