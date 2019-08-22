#include "Shared/DBObjects.h"
#include <algorithm>

using namespace std;

namespace Provider {
ObjectData::ObjectData(const string &p_name, const ObjectType &p_type,
                       const string &p_scheme,
                       const vector<string> &p_params_vector) {
  name = p_name;
  type = p_type;
  schema = p_scheme;
  params = p_params_vector;
}

bool ObjectData::operator==(ObjectData &object) const {
  return (this->name == object.name) && (this->type == object.type);
}

ScriptDefinition::ScriptDefinition(const string &p_name,
                                   const ObjectType &p_type,
                                   const string &p_scheme,
                                   const vector<string> &p_params_vector,
                                   string p_text)
    : ObjectData(p_name, p_type, p_scheme, p_params_vector) {
  text = move(p_text);
}

ScriptDefinition::ScriptDefinition(const ObjectData &object_data,
                                   const string &p_text)
    : ScriptDefinition(object_data.name, object_data.type, object_data.schema,
                       object_data.params, p_text) {}

bool Column::IsNullable() const { return this->nullable_; }

void Column::SetNullable(string value) {
  transform(value.begin(), value.end(), value.begin(), tolower);

  if (value == "yes") {
    this->nullable_ = true;
    return;
  }

  this->nullable_ = false;
}

void Table::SetPartitionTable(string schema, string name,
                              string partition_expression) {
  this->partition_table_.name = move(name);
  this->partition_table_.schema = move(schema);
  this->partition_table_.partition_expression = move(partition_expression);
  this->i_partition_ = true;
}

auto Table::GetPartitionTable() const -> PartitionTable {
  return partition_table_;
}

bool Table::IsPartition() const { return i_partition_; }

std::string CastObjectType(const ObjectType &object_type) {
  switch (object_type) {
  case ObjectType::Empty:
    return string();
  case ObjectType::TableType:
    return "table";
  case ObjectType::FunctionType:
    return "function";
  case ObjectType::TriggerType:
    return "trigger";
  case ObjectType::ViewType:
    return "view";
  case ObjectType::IndexType:
    return "index";
  case ObjectType::SequenceType:
    return "sequence";
  }
}

ObjectType CastObjectType(const std::string &object_type) {
  if (object_type == "table")
    return ObjectType::TableType;
  if (object_type == "function")
    return ObjectType::FunctionType;
  if (object_type == "trigger")
    return ObjectType::TriggerType;
  if (object_type == "view")
    return ObjectType::ViewType;
  if (object_type == "index")
    return ObjectType::IndexType;
  if (object_type == "sequence")
    return ObjectType::SequenceType;

  throw std::invalid_argument("ERROR: No such type in database.\n");
}
} // namespace Provider
