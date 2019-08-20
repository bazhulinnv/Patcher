#ifndef DBPROVIDER_H
#define DBPROVIDER_H

#include "DBProvider/Connection.h"
#include "Shared/DBObjects.h"
#include <pqxx/pqxx>

using namespace std;

// Vector for containing object data
typedef vector<ObjectData> ObjectDataVectorType;

// Vector for containing script data
typedef vector<ScriptDefinition> ScriptDataVectorType;

class DBProvider
{
public:
	explicit DBProvider(string& login_string_pg);

	~DBProvider();

	// Returns all objects of database
	vector<ObjectData> GetObjects() const;

	// Returns script data by object data
	ScriptDefinition GetScriptData(const ObjectData& data, vector<ScriptDefinition>& extra_script_data);

	// Checks if specified object exists in database
	bool DoesCurrentObjectExists(const std::string& scheme, const std::string& signature, const std::string& type) const;

	pqxx::result Query(const std::string& string_PLPG_SQL) const;

	pair<bool, pqxx::result> QueryWithStatus(const string& string_PLPG_SQL) const;

	// Inserts a new record into the content provider
	static void InsertToDB(ObjectData obj);

	// Deletes an existing record from the content provider
	static void DeleteFromDB(ObjectData obj);

	// Updates an existing record from the content provider
	static void Update(ObjectData obj);

	// Returns the MIME type of the data at the given URI
	static ObjectDataVectorType GetType(ObjectData obj);

	// Uses specified view
	static std::vector<ObjectData> UseViewToGetData(std::string name_of_view);

	// Creates new view
	static std::vector<ObjectData> CreateAndUseView(std::string name_of_view, std::string body_of_view);

	bool TableExists(const std::string& schema, const std::string& tableName) const;

	bool SequenceExists(const std::string& schema, const std::string& sequence_name) const;

	bool FunctionExists(const std::string& schema, const std::string& func_signature) const;

	bool IndexExists(const std::string& schema, const std::string& index_name) const;

	bool ViewExists(const std::string& table_schema, const std::string& table_name) const;

	bool TriggerExists(const std::string& trigger_schema, const std::string& trigger_name) const;


private:
	// DBConnectionPool Pool
	shared_ptr<DBConnection::Connection> current_connection_;

	// Getting information about object from database
	Table GetTable(const ObjectData& data) const;

	Function GetFunction(const ObjectData& data) const;

	Trigger GetTrigger(const ObjectData& data) const;

	Sequence GetSequence(const ObjectData& data,
						 int start_value = 1,
						 int minimum_value = 1,
						 int maximum_value = 2147483647,
						 int increment = 1,
						 std::string cycle_option = "NO",
						 std::string comment = "") const;

	View GetView(const ObjectData& data) const;

	Index GetIndex(const ObjectData& data);

	// Get single value from Query
	string GetValue(const string& query_string, const string& column_name) const;

	// Get ScriptDefinition for current type
	ScriptDefinition GetTableData(const ObjectData& data, vector<ScriptDefinition>& extra_script_data) const;

	ScriptDefinition GetFunctionData(const ObjectData& data) const;

	ScriptDefinition GetViewData(const ObjectData& data) const;

	ScriptDefinition GetSequenceData(const ObjectData& data) const;

	ScriptDefinition GetTriggerData(const ObjectData& data) const;

	ScriptDefinition GetIndexData(const ObjectData& data);

	// Methods for initialization of Table structure
	bool InitializePartitionTable(Table& table, const ObjectData& data) const;
	void InitializeType(Table& table, const ObjectData& data) const;
	void InitializeOwner(Table& table, const ObjectData& data) const;
	void InitializeDescription(Table& table, const ObjectData& data) const;
	void InitializeOptions(Table& table, const ObjectData& data) const;
	void InitializeSpace(Table& table, const ObjectData& data) const;
	void InitializeColumns(Table& table, const ObjectData& data) const;
	void InitializePartitionExpression(Table& table, const ObjectData& data) const;
	void InitializeConstraints(Table& table, const ObjectData& data) const;
	void InitializeInheritTables(Table& table, const ObjectData& data) const;
	void InitializeIndexExpressions(Table& table, const ObjectData& data) const;
};

void PrintObjectsData(const pqxx::result& query_result);

#endif