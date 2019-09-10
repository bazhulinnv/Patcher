#ifndef DBPROVIDER_H
#define DBPROVIDER_H

#include "DBProvider/DBConnection.h"
#include <pqxx/pqxx>

using namespace std;

namespace Provider {

/**
 * @brief Describes all object types.
 */
enum class ObjectType {
  Empty,
  Table,
  Function,
  Trigger,
  View,
  Index,
  Sequence
};

struct ObjectData // Structure contains object data
{
  std::string name;                    // Name of object
  ObjectType type = ObjectType::Empty; // Type of object
  std::string schema;                  // Scheme of object
  std::vector<std::string> params;     // Params of object

  ObjectData() = default;

  ObjectData(const std::string &p_name, const ObjectType &p_type,
             const std::string &p_scheme,
             const std::vector<std::string> &p_params_vector = {});

  bool operator==(ObjectData &object) const;
};

struct ScriptDefinition : ObjectData // Structure contains script data
{
  std::string text; // Script text

  ScriptDefinition() = default;

  ScriptDefinition(const std::string &p_name, const ObjectType &p_type,
                   const std::string &p_scheme,
                   const std::vector<std::string> &p_params_vector = {},
                   std::string p_text = "");

  ScriptDefinition(const ObjectData &object_data, const std::string &p_text);
};

struct Column // Structure contains information about column of table
{
  std::string name;
  std::string type;
  std::string default_value;
  std::string description;
  bool IsNullable() const;
  void SetNullable(std::string value);

private:
  bool nullable_ = false;
};

struct Constraint {
  std::string type;
  std::string name;
  std::string column_name;
  std::string check_clause;
  std::string foreign_table_schema;
  std::string foreign_table_name;
  std::string foreign_column_name;
  std::string match_option;
  std::string on_delete;
  std::string on_update;
};

struct PartitionTable {
  std::string schema;
  std::string name;
  std::string partition_expression;
};

struct TableStructure // Structure contains table structure information
{
  std::string type;
  std::string owner;
  std::string description;
  std::string options;
  std::string space;
  std::string partition_expression;

  void SetPartitionTable(std::string schema, std::string name,
                         std::string partition_expression);
  PartitionTable GetPartitionTable() const;
  bool IsPartition() const;

  std::vector<Column> columns;
  std::vector<Constraint> constraints;
  std::vector<std::string> inherit_tables;
  std::vector<std::string> index_create_expressions;

private:
  PartitionTable partition_table_;
  bool i_partition_ = false;
};

std::string CastObjectType(const ObjectType &object_type);

ObjectType CastObjectType(const std::string &object_type);

// Vector for containing object data
typedef vector<ObjectData> ObjectDataVectorType;

// Vector for containing script data
typedef vector<ScriptDefinition> ScriptDataVectorType;

/**
 * @brief Provides various methods of working with the database.
 * Using pqxx library for PostgreSQL.
 */
class DBProvider {
public:
  // Default constructor
  DBProvider();

  explicit DBProvider(const string &connection_params);

  explicit DBProvider(shared_ptr<DBConnection> already_set_connection);

  ~DBProvider();

  void InitializeStatements();

  void PrepareAllStatements();

  void PrepareStatement(const std::string &key,
                        const std::string &statement_definition);

  void PrepareStatements(const std::map<std::string, std::string> &statements);

  void
  ExecutePreparedWithoutArgs(const std::vector<std::string> &statement_keys);

  // Returns all objects of database
  vector<ObjectData> GetObjects() const;

  // Returns script data by object data
  ScriptDefinition
  GetScriptData(const ObjectData &data,
                vector<ScriptDefinition> &extra_script_data) const;

  // Checks if specified object exists in database
  bool DoesCurrentObjectExists(const std::string &scheme,
                               const std::string &signature,
                               const std::string &type) const;

  pqxx::result Query(const std::string &sql_request) const;

  pair<bool, pqxx::result> QueryWithStatus(const string &sql_request) const;

  // Returns the MIME type of the data at the given URI
  static ObjectDataVectorType GetType(ObjectData obj);

  // Uses specified view
  static std::vector<ObjectData> UseViewToGetData(std::string name_of_view);

  // Creates new view
  static std::vector<ObjectData> CreateAndUseView(std::string name_of_view,
                                                  std::string body_of_view);

  bool TableExists(const std::string &schema,
                   const std::string &tableName) const;

  bool SequenceExists(const std::string &schema,
                      const std::string &sequence_name) const;

  bool FunctionExists(const std::string &schema,
                      const std::string &func_signature) const;

  bool IndexExists(const std::string &schema,
                   const std::string &index_name) const;

  bool ViewExists(const std::string &table_schema,
                  const std::string &table_name) const;

  bool TriggerExists(const std::string &trigger_schema,
                     const std::string &trigger_name) const;

private:
  // DBConnectionPool Pool
  shared_ptr<DBConnection> current_connection_;

  std::map<std::string, std::string> prepared_statements_;

  // Getting information about object from database
  TableStructure GetTable(const ObjectData &data) const;

  ScriptDefinition GetFunctionDefinition(const ObjectData &data) const;

  ScriptDefinition GetTriggerDefinition(const ObjectData &data,
                                        const string &comment = "",
                                        const string &code = "") const;

  ScriptDefinition
  GetSequenceDefinition(const ObjectData &data, int start_value = 1,
                        int minimum_value = 1, int maximum_value = 2147483647,
                        int increment = 1, std::string cycle_option = "NO",
                        std::string comment = "") const;

  ScriptDefinition GetViewDefinition(const ObjectData &data) const;

  ScriptDefinition GetIndexDefinition(const ObjectData &data) const;

  // Get single value from Query
  string GetValue(const string &query_string, const string &column_name) const;

  // Get ScriptDefinition for current type
  ScriptDefinition
  GetTableData(const ObjectData &data,
               vector<ScriptDefinition> &extra_script_data) const;

  ScriptDefinition GetFunctionData(const ObjectData &data) const;

  ScriptDefinition GetViewData(const ObjectData &data) const;

  ScriptDefinition GetSequenceData(const ObjectData &data) const;

  ScriptDefinition GetTriggerData(const ObjectData &data) const;

  ScriptDefinition GetIndexData(const ObjectData &data) const;

  // Methods for initialization of TableStructure structure
  bool InitializePartitionTable(TableStructure &table,
                                const ObjectData &data) const;
  void InitializeType(TableStructure &table, const ObjectData &data) const;
  void InitializeOwner(TableStructure &table, const ObjectData &data) const;
  void InitializeDescription(TableStructure &table,
                             const ObjectData &data) const;
  void InitializeOptions(TableStructure &table, const ObjectData &data) const;
  void InitializeSpace(TableStructure &table, const ObjectData &data) const;
  void InitializeColumns(TableStructure &table, const ObjectData &data) const;
  void InitializePartitionExpression(TableStructure &table,
                                     const ObjectData &data) const;
  void InitializeConstraints(TableStructure &table,
                             const ObjectData &data) const;
  void InitializeInheritTables(TableStructure &table,
                               const ObjectData &data) const;
  void InitializeIndexExpressions(TableStructure &table,
                                  const ObjectData &data) const;
};

void PrintObjectsData(const pqxx::result &query_result);
} // namespace Provider

#endif