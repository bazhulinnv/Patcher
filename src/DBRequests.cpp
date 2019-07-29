#include "DBProvider/DBRequests.h"
#include "Shared/ParsingTools.h"

#include<iostream>
#include<pqxx/pqxx>

using namespace std;

pqxx::result getFunctionsFromSchema(DBProvider* dbProv, const string& specificSchema)
{
	string q =
		ParsingTools::interpolate(
			"SELECT r.specific_name, r.routine_name||'('||COALESCE(array_to_string(p.proargnames, ',', '*'),'')||')' "
			"FROM information_schema.routines r, pg_catalog.pg_proc p "
			"WHERE r.specific_schema='${}' "
			"AND r.external_language = 'PLPGSQL' "
			"AND r.routine_name = p.proname and r.specific_name = p.proname||'_'||p.oid", specificSchema);

	return dbProv->query(q);
}

pqxx::result getFunctionsFromSchema(DBProvider* dbProv, const string& specificSchema)
{
	string q =
		ParsingTools::interpolate(
			"SELECT r.specific_name, r.routine_name||'('||COALESCE(array_to_string(p.proargnames, ',', '*'),'')||')' "
			"FROM information_schema.routines r, pg_catalog.pg_proc p "
			"WHERE r.specific_schema='${}' "
			"AND r.external_language = 'PLPGSQL' "
			"AND r.routine_name = p.proname and r.specific_name = p.proname||'_'||p.oid", specificSchema);

	return dbProv->query(q);
}


bool tableExists(DBProvider* dbProv, const string& tableSchema, const string& tableName)
{
	// Define SQL request
	string q =
		ParsingTools::interpolateAll(
			"SELECT EXISTS (SELECT * "
			"FROM information_schema.tables "
			"WHERE table_schema = '${}' "
			"AND table_name = '${}');",
			vector<string> { tableSchema, tableName });

	auto queryResult = dbProv->query(q);
	return queryResult.begin()["exists"].as<bool>();
}

bool sequenceExists(DBProvider* dbProv, const string& sequenceSchema, const string& sequenceName)
{
	string q =
		ParsingTools::interpolateAll(
			"SELECT EXISTS (SELECT * "
			"FROM information_schema.sequences "
			"WHERE sequence_schema = '${}' "
			"AND sequence_name = '${}');",
			vector<string> { sequenceSchema, sequenceName });

	auto queryResult = dbProv->query(q);
	return queryResult.begin()["exists"].as<bool>();
}

bool functionExists(DBProvider* dbProv, const string& name)
{
	return true;
}

bool indexExists(DBProvider* dbProv, const string& name)
{
	return true;
}

bool viewExists(DBProvider* dbProv, const string& tableSchema, const string& tableName)
{
	string q =
		ParsingTools::interpolateAll(
			"SELECT EXISTS (SELECT * "
			"FROM information_schema.views "
			"WHERE table_schema = '${}' "
			"AND table_name = '${}');",
			vector<string> { tableSchema, tableName });

	auto queryResult = dbProv->query(q);
	return queryResult.begin()["exists"].as<bool>();
}

bool triggerExists(DBProvider* dbProv, const string& triggerSchema, const string& triggerName)
{
	string q =
		ParsingTools::interpolateAll(
			"SELECT EXISTS (SELECT * "
			"FROM information_schema.triggers "
			"WHERE trigger_schema = '${}' "
			"AND trigger_name = '${}');",
			vector<string> { triggerSchema, triggerName });

	auto resOfQuery = dbProv->query(q);
	return false;
}
