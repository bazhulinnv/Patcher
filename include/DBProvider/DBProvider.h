#ifndef DBPROVIDER_H
#define DBPROVIDER_H

#include "DBProvider/DBConnection.h"
#include <pqxx/pqxx>

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

struct ObjectData // Structure describes object data
{
  std::string name;                    // Name of object
  ObjectType type = ObjectType::Empty; // Type of object
  std::string schema;                  // Scheme of object
  std::vector<std::string> params;     // Parameters of object

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

// Vector contains objects data.
using ObjectsDataVector = std::vector<ObjectData>;

// Vector contains scripts definitions.
using DefinitionsVector = std::vector<ScriptDefinition> ;

/**
 * @brief Describes column of table
 */
struct Column
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

/**
 * @brief Describes constraints structure.
 */
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

/**
 * @brief Describes partition table structure.
 */
struct PartitionTable {
  std::string schema;
  std::string name;
  std::string partition_expression;
};

struct TableStructure // Structure contains table information
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

// Casts instance of enum class ObjectType to std::string.
std::string CastObjectType(const ObjectType &object_type);

// Casts std::string ObjectType to instance of enum class ObjectType.
ObjectType CastObjectType(const std::string &object_type);

/**
 * @brief Provides various methods of working with the database.
 * Using pqxx library for PostgreSQL.
 */
class DBProvider {
public:
  // Default constructor for DBProvider.
  DBProvider();

  /**
   * @brief Sets up connection parameters explicitly.
   * @param connection_params String containing connection parameters, in format:
   *							< localhost:5432:database:username:password >.
   */
  explicit DBProvider(const std::string & connection_params);

  /**
   * @brief Sets up connection explicitly.
   * @param already_set_connection Pointer to DBConnection which was already set.
   */
  explicit DBProvider(std::shared_ptr<DBConnection> already_set_connection);

   // Safely deletes DBProvider with its state.
  ~DBProvider();

  /**
   * @brief Prepares standard statements frequently used by DBProvider.
   */
  void PrepareAllStatements();

  /**
   * @brief Prepares statement
   * @param key 
   * @param statement_definition 
   */
  void PrepareStatement(const std::string &key,
                        const std::string &statement_definition) const;

  /**
   * @brief Prepares custom statements (queries) for future use.
   * @param statements Map describes all custom statements.
   */
  void PrepareStatements(const std::map<std::string, std::string> &statements) const;

  /**
   * @brief Executes already prepared statement by given statement key. 
   * @param statement_keys Statement identifier.
   */
  void ExecutePreparedWithoutArgs(const std::vector<std::string> &statement_keys) const;

  /**
   * @brief Gets all objects from database.
   * @return Vector containing all object descriptors with metadata.
   */
  std::vector<ObjectData> GetObjects() const;

  // Returns script data for specified object data
  ScriptDefinition GetScriptData(const ObjectData &data,
                std::vector<ScriptDefinition> &extra_script_data) const;
  /**
   * @brief Checks existence of an object in specified schema.
   * @param scheme Schema to check.
   * @param signature Full object name or signature.
   * @param type Type of an object. Can be: Table, Function, Trigger, View, Index, Sequence.
   * @return True if object exists in specified schema, otherwise returns false.
   */
  bool DoesCurrentObjectExists(const std::string &scheme,
                               const std::string &signature,
                               const std::string &type) const;

  /**
   * @brief Performs query request.
   * @param sql_request PLPG SQL request to perform.
   * @return Object of type pqxx::result containing all affected queries.
   */
  pqxx::result Query(const std::string &sql_request) const;

  /**
   * @brief Tries to performs query request, if request succeeded returns with boolean status 'True'.
   * @param sql_request PLPG SQL request to perform.
   * @return Object containing query result and query status.
   */
  std::pair<bool, pqxx::result> QueryWithStatus(const std::string &sql_request) const;

  /**
   * @brief Checks existence of the table in specified schema.
   * @param table_schema Schema to check.
   * @param table_name Name of the table to check.
   * @return True if table exists in specified schema, otherwise returns false.
   */
  bool TableExists(const std::string &table_schema,
                   const std::string &table_name) const;

  /**
   * @brief Checks existence of the sequence in specified schema.
   * @param sequence_schema Schema to check.
   * @param sequence_name Name of the table to check.
   * @return True if sequence exists in specified schema, otherwise returns false.
   */
  bool SequenceExists(const std::string &sequence_schema,
                      const std::string &sequence_name) const;

  /**
   * @brief Checks existence of the function in specified schema.
   * @param function_schema Schema to check.
   * @param function_signature Full function signature to check, in format:
				<FunctionName(parameter_1, parameter_1, ...)>.
   * @return True if function exists in specified schema, otherwise returns false.
   */
  bool FunctionExists(const std::string &function_schema,
                      const std::string &function_signature) const;

  /**
   * @brief Checks existence of the index in specified schema.
   * @param index_schema Schema to check.
   * @param index_name Name of the index to check.
   * @return True if index exists, otherwise returns false.
   */
  bool IndexExists(const std::string &index_schema,
                   const std::string &index_name) const;

  /**
   * @brief Checks existence of the view in specified schema.
   * @param view_schema Schema to check.
   * @param view_name Name of the view to check.
   * @return True if view exists, otherwise returns false.
   */
  bool ViewExists(const std::string &view_schema,
                  const std::string &view_name) const;

  /**
   * @brief Checks existence of the trigger in specified schema.
   * @param trigger_schema Schema to check.
   * @param trigger_name Name of the trigger to check.
   * @return True if trigger exists, otherwise returns false.
   */
  bool TriggerExists(const std::string &trigger_schema,
                     const std::string &trigger_name) const;

private:
  // Points to current connection
  std::shared_ptr<DBConnection> current_connection_;

  // Keeps prepared statement
  std::map<std::string, std::string> prepared_statements_;
  
  /**
   * @brief Initializes map of standard statements (queries) frequently used by DBProvider.
   */
  void InitializeStatements();

  // Getting information about object from database
  TableStructure GetTable(const ObjectData &data) const;

  ScriptDefinition FunctionDefinition(const ObjectData &data) const;

  ScriptDefinition TriggerDefinition(const ObjectData &data,
                                        const std::string &comment = "",
                                        const std::string &code = "") const;

  ScriptDefinition
  SequenceDefinition(const ObjectData &data, int start_value = 1,
                        int minimum_value = 1, int maximum_value = 2147483647,
                        int increment = 1, bool cycle_option = false,
                        const std::string& comment = "") const;

  ScriptDefinition ViewDefinition(const ObjectData &data) const;

  ScriptDefinition IndexDefinition(const ObjectData &data) const;

  // Gets single value from Query.
  std::string GetValue(const std::string &sql_request, const std::string &column_name) const;

  // Gets ScriptDefinition for current type.
  ScriptDefinition
  GetTableData(const ObjectData &data,
               std::vector<ScriptDefinition> &extra_script_data) const;

  // Methods for initialization of TableStructure structure

  bool InitializePartitionTable(TableStructure &table, const ObjectData &data) const;
  void InitializeType(TableStructure &table, const ObjectData &data) const;
  void InitializeOwner(TableStructure &table, const ObjectData &data) const;
  void InitializeDescription(TableStructure &table, const ObjectData &data) const;
  void InitializeOptions(TableStructure &table, const ObjectData &data) const;
  void InitializeSpace(TableStructure &table, const ObjectData &data) const;
  void InitializeColumns(TableStructure &table, const ObjectData &data) const;
  void InitializePartitionExpression(TableStructure &table, const ObjectData &data) const;
  void InitializeConstraints(TableStructure &table, const ObjectData &data) const;
  void InitializeInheritTables(TableStructure &table, const ObjectData &data) const;
  void InitializeIndexExpressions(TableStructure &table, const ObjectData &data) const;
};

void PrintObjectsData(const pqxx::result &query_result);
} // namespace Provider

#endif