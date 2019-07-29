#ifndef DBPROVIDER_H
#define DBPROVIDER_H

#include "DBProvider/DBConnection.h"

#include <pqxx/pqxx>
#include <tuple>
#include <string>

struct ObjectData // Sctruct for containing objet data
{
	std::string name; // Name of object
	std::string type; // Type of object
	std::string scheme; // Scheme of object
	std::vector<std::string> paramsVector; // Params of object

	ObjectData() {}
	ObjectData(std::string pName, std::string pType, std::string pScheme, std::vector<std::string> pParamsVector)
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

struct ScriptData : ObjectData // Sctruct for containing script data
{
	std::string text; // Script text

	ScriptData()
	{
	}
	
	ScriptData(std::string pName, std::string pType, std::string pScheme, std::vector<std::string> pParamsVector, std::string pText = "") : ObjectData(pName, pType, pScheme, pParamsVector)
	{
		text = pText;
	}
	
	ScriptData(ObjectData objectData, std::string pText = "") : ScriptData(objectData.name, objectData.type, objectData.scheme, objectData.paramsVector, pText) {}
};

// Vector for containing object data
typedef std::vector<ObjectData> objectDataVectorType;

// Vector for containing script data
typedef std::vector<ScriptData> scriptDataVectorType;

class DBProvider
{
public:
	explicit DBProvider(std::string args);
	
	~DBProvider();
	
	// Returns all objects of database
	std::vector<ObjectData> getObjects();
	
	// Returns script data by object data
	ScriptData getScriptData(ObjectData);
	
	// Checks if specified object exists in database
	bool doesCurrentObjectExists(std::string scheme, std::string name, std::string type)
	{
		return true;
	}
	
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

private:
	DBConnection *_connection = nullptr;
};

void printObjectsData(pqxx::result res);

#endif