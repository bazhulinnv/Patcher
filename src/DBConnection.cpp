#include "DBProvider/DBConnection.h"
#include <iostream>
#include <stdexcept>

using namespace std;

DBConnection::DBConnection() = default;

DBConnection::DBConnection(string &connection_str) {
  connection_params_ = LoginData(connection_str);
  is_parameters_set_ = true;
}

bool DBConnection::IsOpen() const {
  if (db_connection_ == nullptr) {
    return false;
  }
  return db_connection_->is_open();
}

DBConnection::~DBConnection() { CloseConnection(); }

void DBConnection::SetConnection(string &connection_str) {

  // If connection already exists
  // then close existing connection and set new
  if (is_connected_) {
    CloseConnection();
    is_connected_ = false;
  }

  connection_params_ = LoginData(connection_str);
  is_parameters_set_ = true;
}

void DBConnection::Connect() {
  if (!is_parameters_set_) {
    throw runtime_error(
        "ERROR: Tried to access parameters, but parameters weren't"
        "set properly in constructor or by calling"
        "\"SetConnection(\"localhost:5432:MyDatabase:user:password\")\".");
  }

  try {
    db_connection_ =
        make_shared<pqxx::connection>(connection_params_.LoginString_Pqxx());
    db_connection_->activate();
    is_connected_ = true;
  } catch (exception &err) {
    cerr << err.what() << endl;
    throw runtime_error("ERROR: Couldn't establish connection.\n\tparameters:" +
                        connection_params_.LoginString_Pqxx());
  }
}

LoginData DBConnection::GetParameters() {
  if (!is_parameters_set_) {
    throw runtime_error("ERROR: Tried to get parameters, but parameters weren't"
                        "set properly in constructor or by "
                        "\"SetConnection(\"YOUR DB PARAMETERS\")\".");
  }

  return connection_params_;
}

shared_ptr<pqxx::connection_base> DBConnection::GetConnection() const {
  if (!is_connected_) {
    throw runtime_error(
        R"(ERROR: Tried to get pqxx::connection_base, but parameters weren't"
					"set properly in constructor or by "SetConnection".)");
  }

  return db_connection_;
}

bool DBConnection::IsParametersSet() const { return is_parameters_set_; }

void DBConnection::CloseConnection() {
  is_parameters_set_ = false;
  is_connected_ = false;
  if (db_connection_ != nullptr) {
    db_connection_->disconnect();
    db_connection_.reset();
  }
}
