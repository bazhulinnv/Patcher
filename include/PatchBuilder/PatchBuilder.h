#ifndef PATCHBUILDER_H
#define PATCHBUILDER_H

#include <string>
#include <vector>
#include <utility>
#include "DBProvider/DBProvider.h"

using namespace std;

struct ObjectData // Sctruct for containing objet data
{
	string name;
	string type;

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
  PatchBuilder(const string pPatchListFullName); // Constructor
  ~PatchBuilder(); // distructor
  void buildPatch(const string directory); // Build patch in choosen directory

private:
	string patchListFullName; // Directory and name of PatchList
	const string dependencyListName = "DependencyList.txt"; // Name of file that will be created. Contains list of objects and their type that required to install the patch 
	scriptDataVectorType getScriptDataVector() /*const*/; // Getting vector of scripts, created by DBProvider
	objectDataVectorType getObjectDataVector() const; // Getting vector of objects from source database
	void creatInstallPocket(const string directory, const scriptDataVectorType &scriptDataVector) const; // Creating sql files for scripts from scriptDataVector and creating install script file
	bool isContains(const ObjectData data, const string &scriptText) const; // Returns true, if the object was found in the script
	objectDataVectorType getPatchListVector() const; // Getting vector of objects that contains a patch
	void fillScriptDataVector(scriptDataVectorType &scriptDataVector); // temp
	void remove(objectDataVectorType &objectDataVector_first, const objectDataVectorType &objectDataVector_second); // Removing elements of second vector from first vector
	void removeCommits(scriptDataVectorType &scriptDataVector); // Removing all commits from script text
};
#endif