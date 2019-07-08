#include <DBProvider/DBProvider.h>

DBProvider::DBProvider() {}
DBProvider::~DBProvider() {}
int DBProvider::getObjects() { return 0; }
void DBProvider::connect() {}

bool DBProvider::isCurrentObjectExist(std::string objectName, std::string objectType)
{
	return false;
}



