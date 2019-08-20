#ifndef DBOBJECTS_H
#define DBOBJECTS_H

#include <string>
#include <vector>

struct ObjectData // Structure contains object data
{
	std::string name; // Name of object
	std::string type; // Type of object
	std::string schema; // Scheme of object
	std::vector<std::string> params; // Params of object

	ObjectData() = default;
	ObjectData(const std::string& p_name,
			   const std::string& p_type,
			   const std::string& p_scheme, std::vector<std::string> p_params_vector = {});

	bool operator ==(ObjectData& object) const;
};

struct ScriptDefinition : ObjectData // Structure contains script data
{
	std::string text; // Script text

	ScriptDefinition() = default;

	ScriptDefinition(const std::string& p_name,
					 const std::string& p_type,
					 const std::string& p_scheme, std::vector<std::string> p_params_vector = {},
					 std::string p_text = "");

	ScriptDefinition(const ObjectData& object_data, const std::string& p_text);
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

struct Constraint
{
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

struct PartitionTable
{
	std::string schema;
	std::string name;
	std::string partition_expression;
};

struct Table // Structure contains table structure information
{
	std::string type;
	std::string owner;
	std::string description;
	std::string options;
	std::string space;
	std::string partition_expression;

	void SetPartitionTable(std::string schema, std::string name, std::string partition_expression);
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

struct Function : ObjectData
{
	std::string specific_catalog;
	std::string specific_schema;
	std::string specific_name;

	std::string routine_catalog;
	std::string routine_schema;
	std::string routine_name;
	std::string routine_type;

	std::string data_type;
	std::string external_language;

	std::vector<std::string> function_parameters;
	std::string function_text_definition;
};

struct Trigger : ObjectData
{
	std::string trigger_catalog;
	std::string trigger_schema;
	std::string trigger_name;
	std::string string_agg;
	std::string trigger_text_definition;
};

struct Index : ObjectData
{
	std::string schema_name;
	std::string table_name;
	std::string index_name;
	std::string index_text_definition;
};

struct View : ObjectData
{
	std::string table_catalog;
	std::string table_schema;
	std::string table_name;
	std::string check_option;
	std::string is_updatable;
	std::string is_insertable_into;
	std::string is_trigger_updatable;
	std::string is_trigger_deletable;
	std::string is_trigger_insertable_into;

	std::string view_text_definition;
};

struct Sequence : ObjectData
{
	std::string sequence_catalog;
	std::string sequence_schema;
	std::string sequence_name;
	std::string data_type;

	int numeric_precision = 32;
	int numeric_precision_radix = 2;
	int numeric_scale = 0;

	std::string start_value;
	std::string minimum_value;
	std::string maximum_value;
	std::string increment;
	std::string cycle_option;

	std::string query_create_seq;
};

#endif