#ifndef LOGINDATA_H
#define LOGINDATA_H

#include <string>

// Keeps login credentials
struct LoginData {
  std::string hostname;
  int port = 5432;
  std::string database;
  std::string username;
  std::string password;

  LoginData() = default;

  explicit LoginData(std::string hostname_, unsigned int port_,
                     std::string database_, std::string username_,
                     std::string password_);

  explicit LoginData(const std::string &pgpass_string_);

  // Returns login data as single string in libpqxx format
  // e.g. "hostname=127.0.0.1 port=5432 dbname=example username=user
  // password=qwerty123"
  std::string LoginString_Pqxx() const;

  // Returns login data as single string (PGPASSFILE format)
  // e.g. "hostname:port:database:username:password"
  std::string LoginString_PG() const;
};

#endif