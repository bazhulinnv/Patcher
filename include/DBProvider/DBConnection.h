#ifndef DBCONNECTION_H
#define DBCONNECTION_H

#include "DBProvider/LoginData.h"
#include <memory>
#include <pqxx/pqxx>
#include <string>

/**
 * @brief Implements connection to database using pqxx::connection from pqxx
 * library.
 */
class DBConnection {
public:
  // Default constructor
  DBConnection();

  // Constructor explicitly sets parameters to database
  explicit DBConnection(std::string &connection_str);

  /**
   * @brief Checks if connection parameters was set.
   * @return True if parameters was set, otherwise false
   */
  bool IsParametersSet() const;

  /**
   * @brief Checks if pqxx::connection is open.
   * @return True if connection opened, otherwise false.
   */
  bool IsOpen() const;

  // Closes then destroys database connection
  ~DBConnection();

  /**
   * @brief Sets or resets database connection with given parameters.
   * @param connection_str String contains all parameters in "pgpass" format.
   * @throws std::invalid_argument if could not parse parameters.
   */
  void SetConnection(std::string &connection_str);

  /**
   * @brief Tries connecting to database with already set parameters.
   * @throws std::runtime_error if connection failed.
   */
  void Connect();

  /**
   * @brief Gets connection parameters. If parameters were not set throws
   * exception.
   * @return LoginData which represent parameters.
   * @throws std::runtime_error
   */
  LoginData GetParameters();

  /**
   * @brief Gets pqxx connection object. If connection was not set throws
   * exception.
   * @return Shared pointer to pqxx::connection_base object.
   * pqxx::connection_base is base class for pqxx::lazyconnection and
   * pqxx::connection.
   * @throws std::runtime_error
   */
  std::shared_ptr<pqxx::connection_base> GetConnection() const;

private:
  bool is_parameters_set_ = false;
  bool is_connected_ = false;
  LoginData connection_params_;
  std::shared_ptr<pqxx::connection_base> db_connection_;

  /**
   * @brief Safely closes connection.
   */
  void CloseConnection();
};

#endif
