#include<pqxx/pqxx>
#include<string>
#include<iostream>
#include "DBProvider/DBProvider.h"

bool tableExists(DBProvider* dbProv);

bool sequenceExists(DBProvider* dbProv);

bool functionExists(DBProvider* dbProv);

bool viewExists(DBProvider* dbProv);

bool triggerExists(DBProvider* dbProv);