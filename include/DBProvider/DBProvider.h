#ifndef DBPROVIDER_H
#define DBPROVIDER_H
#include <string>

#include <DBProvider/DBConnection.h>
#include <tuple>
#include <pqxx/pqxx>
#include <string>

using namespace std;

struct ObjectData // Sctruct for containing objet data
{
	string name; // Name of object
	string type; // Type of object
	string scheme; // Scheme of object
	vector<string> paramsVector; // Params of object

	ObjectData() {}
	ObjectData(string pName, string pType, string pScheme, vector<string> pParamsVector)
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
	string text; // Script text

	ScriptData()
	{
	}
	
	ScriptData(string pName, string pType, string pScheme, vector<string> pParamsVector, string pText = "") : ObjectData(pName, pType, pScheme, pParamsVector)
	{
		text = pText;
	}
	
	ScriptData(ObjectData objectData, string pText = "") : ScriptData(objectData.name, objectData.type, objectData.scheme, objectData.paramsVector, pText) {}
};

// Vector for containing object data
typedef vector<ObjectData> objectDataVectorType;

// Vector for containing script data
typedef vector<ScriptData> scriptDataVectorType;

void printObjectsData(pqxx::result res);

class DBProvider
{
public:
	explicit DBProvider(std::string args);
	~DBProvider();
	
	// Returns all objects of database
	vector<ObjectData> getObjects();
	
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

	// Use specified view
	vector<ObjectData> useViewToGetData(std::string nameOfView)
	{
	}

	// Create new view
	vector<ObjectData> createAndUseView(std::string nameOfView, std::string bodyOfView)
	{
	}

private:
	DBConnection *conn = nullptr;
};

#endif