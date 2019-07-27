#include "..\include\DBProvider\DBRequests.h"

bool tableExists(DBProvider* dbProv)
{
	std::string q = "SELECT EXISTS (SELECT * FROM information_schema.tables WHERE table_schema = ?"
		"AND table_name = ?)";
	auto resOfQuery = dbProv->query(q);
	return false;
}

bool sequenceExists(DBProvider* dbProv)
{
	std::string q = "SELECT EXISTS (SELECT * FROM information_schema.sequences WHERE sequence_schema = ?"
		"AND sequence_name = ?)";
	auto resOfQuery = dbProv->query(q);
	return false;
}

bool functionExists(DBProvider* dbProv)
{
	return true;
}

bool viewExists(DBProvider* dbProv)
{
	std::string q = "SELECT EXISTS (SELECT * FROM information_schema.views WHERE table_schema = ?"
		"AND table_name = ?)";
	auto resOfQuery = dbProv->query(q);
	return false;
}

bool triggerExists(DBProvider* dbProv)
{

	std::string q = "SELECT EXISTS (SELECT * FROM information_schema.triggers WHERE trigger_schema = ?"
		"AND trigger_name = ?)";
	auto resOfQuery = dbProv->query(q);
	return false;
}
