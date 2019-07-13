#ifndef PATCHBUILDER_H
#define PATCHBUILDER_H

#include <string>
#include <vector>
#include <utility>
#include <regex>
#include "DBProvider/DBProvider.h"

// Templates code words
constexpr auto TYPE_CODE = "$type$"; // Type code
constexpr auto BEGIN_CODE = "$begin$"; // Begining of the block code
constexpr auto END_CODE = "$end$"; // Ending of the block code
constexpr auto NAME_CODE = "%name%"; // Object name code
constexpr auto NAME_LENGTH = 6; // Object name length
constexpr auto SCHEMA_CODE = "%schema%"; // Object name code
constexpr auto SCHEMA_LENGTH = 8; // Object name length
constexpr auto ANY_TYPE_CODE = "$any$"; // Type not present in the template code

constexpr auto DEPENDENCY_LIST_NAME = "DependencyList.txt"; // Name of file that will be created. Contains list of objects and their type that required to install the patch
constexpr auto INSTALL_SCRIPT_NAME_BAT = "Install.bat"; // Name of PatchList with .bat format
constexpr auto INSTALL_SCRIPT_NAME_SH = "Install.sh"; // Name of PatchList with .shell format

using namespace std;

struct ObjectData // Sctruct for containing objet data
{
	string name;
	string type;
	string schema;

	bool operator == (ObjectData &object) const {
		return (this->name == object.name) && (this->type == object.type);
	}
};
struct ScriptData : ObjectData // Sctruct for containing script data
{
	string text;
};
typedef vector<ObjectData> objectDataVectorType; // Vector for containing object data
typedef vector<ScriptData> scriptDataVectorType; // Vector for containing script data

class PatchBuilder 
{
public:
  PatchBuilder(const string pPatchListFullName, const string pTemplatesFullName, const string userName, const string databaseName); // Constructor
  ~PatchBuilder(); // distructor
  void buildPatch(const string directory); // Build patch in choosen directory

private:
	string patchListFullName; // Directory and name of PatchList
	string templatesFullName; // Directory and name of Templates file
	string userName; // Name of user, who want to build patch
	string databaseName; // Name ob database
	scriptDataVectorType getScriptDataVector() /*const*/; // Getting vector of scripts, created by DBProvider
	objectDataVectorType getObjectDataVector() const; // Getting vector of objects from source database
	void creatInstallPocket(const string directory, const scriptDataVectorType &scriptDataVector) const; // Creating sql files for scripts from scriptDataVector and creating install script file
	bool isContains(const ObjectData data, const string &scriptText); // Returns true, if the object was found in the script
	objectDataVectorType getPatchListVector() const; // Getting vector of objects that contains a patch
	void fillScriptDataVector(scriptDataVectorType &scriptDataVector); // temp
	void remove(objectDataVectorType &objectDataVector_first, const objectDataVectorType &objectDataVector_second); // Removing elements of second vector from first vector
	void removeComments(scriptDataVectorType &scriptDataVector); // Removing all commits from script text
	regex createExpression(ObjectData data); // Creating regular expression for data from params
};
#endif