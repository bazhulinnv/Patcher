#include <DBProvider/DBProvider.h>

DBProvider::DBProvider() {}
DBProvider::~DBProvider() {}
int DBProvider::getObjects() { return 0; }
void DBProvider::connect(char *parameters) {
	//get string with parameters from gui (string looks like hostname:port:database:username:password)
}

bool DBProvider::isCurrentObjectExist(std::string objectName, std::string objectType) {
	return true;
}



