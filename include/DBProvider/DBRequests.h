#include "DBProvider/DBProvider.h"

#include<string>

bool tableExists(DBProvider* dbProv, const std::string& tableSchema, const std::string& tableName);

bool sequenceExists(DBProvider* dbProv, const std::string& sequenceSchema, const std::string& sequenceName);

bool functionExists(DBProvider* dbProv, const std::string& name);

bool indexExists(DBProvider* dbProv, const std::string& name);

bool viewExists(DBProvider* dbProv, const std::string& tableSchema, const std::string& tableName);

bool triggerExists(DBProvider* dbProv, const std::string& triggerSchema, const std::string& triggerName);
