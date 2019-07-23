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

	bool operator == (ObjectData &object) const {
		return (this->name == object.name) && (this->type == object.type);
	}
};
struct ScriptData : ObjectData // Sctruct for containing script data
{
	string text; // Script text

	ScriptData() {}
	ScriptData(string pName, string pType, string pScheme, vector<string> pParamsVector, string pText = "") : ObjectData(pName, pType, pScheme, pParamsVector)
	{
		text = pText;
	}
	ScriptData(ObjectData objectData, string pText = "") :ScriptData(objectData.name, objectData.type, objectData.scheme, objectData.paramsVector, pText) {}
};
typedef vector<ObjectData> objectDataVectorType; // Vector for containing object data
typedef vector<ScriptData> scriptDataVectorType; // Vector for containing script data

void printObjectsData(pqxx::result res);

class DBProvider
{
public:
	explicit DBProvider(std::string args);
	~DBProvider();
	vector<ObjectData> getObjects(); // Returns all objects of database
	ScriptData getScriptData(ObjectData); // Returns script data by object data

	pqxx::result query(std::string strSQL);

private:
	DBConnection *conn = nullptr;
};

#endif