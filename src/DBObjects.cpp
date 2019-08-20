#include "Shared/DBObjects.h"
#include <algorithm>

using namespace std;

ObjectData::ObjectData(const string& p_name, const string& p_type, const string& p_scheme,
					   const vector<string> p_params_vector)
{
	name = p_name;
	type = p_type;
	schema = p_scheme;
	params = p_params_vector;
}

bool ObjectData::operator==(ObjectData& object) const
{
	return (this->name == object.name) && (this->type == object.type);
}

ScriptDefinition::ScriptDefinition(const string& p_name, const string& p_type, const string& p_scheme,
								   const vector<string> p_params_vector, string p_text) : ObjectData(
									   p_name, p_type, p_scheme, p_params_vector)
{
	text = move(p_text);
}

ScriptDefinition::ScriptDefinition(const ObjectData& object_data, const string& p_text) : ScriptDefinition(
	object_data.name, object_data.type, object_data.schema, object_data.params, p_text)
{
}

bool Column::IsNullable() const
{
	return this->nullable_;
}

void Column::SetNullable(string value)
{
	transform(value.begin(), value.end(), value.begin(), tolower);

	if (value == "yes")
	{
		this->nullable_ = true;
		return;
	}

	this->nullable_ = false;
}

void Table::SetPartitionTable(string schema, string name, string partition_expression)
{
	this->partition_table_.name = move(name);
	this->partition_table_.schema = move(schema);
	this->partition_table_.partition_expression = move(partition_expression);
	this->i_partition_ = true;
}

auto Table::GetPartitionTable() const -> PartitionTable
{
	return partition_table_;
}

bool Table::IsPartition() const
{
	return i_partition_;
}