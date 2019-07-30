#ifndef DBPROVIDER_H
#define DBPROVIDER_H

#include "DBProvider/DBConnection.h"
#include "Shared/ParsingTools.h"

#include <pqxx/pqxx>
#include <tuple>
#include <string>

using namespace std;

struct ObjectData // Sctructure for containing objet data
{
	string name; // Name of object
	string type; // Type of object
	string scheme; // Scheme of object
	vector<string> paramsVector; // Params of object

	ObjectData() = default;
	ObjectData(string pName, string pType, string pScheme, vector<string> pParamsVector = vector<string>())
	{
		name = pName;
		type = pType;
		scheme = pScheme;
		paramsVector = pParamsVector;
	}

	bool operator == (ObjectData &object) const
	{
		return (this->name == object.name) && (this->type == object.type);
	}
};

struct ScriptData : ObjectData // Sctructure for containing script data
{
	std::string text; // Script text

	ScriptData() = default;
	
	ScriptData(std::string pName,string pType,string pScheme,vector<string> pParamsVector, string pText = "") : ObjectData(pName, pType, pScheme, pParamsVector)
	{
		text = pText;
	}
	
	ScriptData(ObjectData objectData, string pText) : ScriptData(objectData.name, objectData.type, objectData.scheme, objectData.paramsVector, pText) {}
};

struct Column // Structure for containing information about column of table
{
public:
	string name;
	string type;
	string defaultValue;
	bool isNullable();
	void setNullabel(string nullable);
	unsigned int maxLength;
private:
	bool nullable_;
};

struct ObectInfo // Sctructure for containing object structure information
{
	vector<Column> columns;
};

// Vector for containing object data
typedef vector<ObjectData> objectDataVectorType;

// Vector for containing script data
typedef vector<ScriptData> scriptDataVectorType;

class DBProvider
{
public:
	explicit DBProvider(std::string args);
	
	~DBProvider();

	// Returns all objects of database
	vector<ObjectData> getObjects();
	
	// Returns script text by object data
	ScriptData getScriptData(const ObjectData &data);
	
	// Checks if specified object exists in database
	bool doesCurrentObjectExists(std::string scheme, std::string name, std::string type);

	pqxx::result query(std::string strSQL);

	// Inserts a new record into the content provider
	void insertToDB(ObjectData obj)
	{
	}

	// Deletes an existing record from the content provider
	void deleteFromDB(ObjectData obj)
	{
	}

	// Updates an existing record from the content provider
	void update(ObjectData obj)
	{
	}

	// Returns the MIME type of the data at the given URI
	objectDataVectorType getType(ObjectData obj)
	{
	}

	// Uses specified view
	std::vector<ObjectData> useViewToGetData(std::string nameOfView)
	{
	}

	// Creates new view
	std::vector<ObjectData> createAndUseView(std::string nameOfView, std::string bodyOfView)
	{
	}

	bool tableExists(const std::string& tableSchema, const std::string& tableName);

	bool sequenceExists(const std::string& sequenceSchema, const std::string& sequenceName);

	bool functionExists(const std::string& name);

	bool indexExists(const std::string& name);

	bool viewExists(const std::string& tableSchema, const std::string& tableName);

	bool triggerExists(const std::string& triggerSchema, const std::string& triggerName);

private:
	DBConnection *_connection = nullptr;

};

void printObjectsData(pqxx::result res);

#endif