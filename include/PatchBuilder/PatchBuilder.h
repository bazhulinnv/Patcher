#ifndef PATCHBUILDER_H
#define PATCHBUILDER_H

#include "DBProvider/DBProvider.h"
#include <regex>
#include <string>
#include <vector>
using namespace Provider;
// Templates code words
constexpr auto TYPE_CODE = "$type$";     // Type code
constexpr auto BEGIN_CODE = "$begin$";   // Begining of the block code
constexpr auto END_CODE = "$end$";       // Ending of the block code
constexpr auto NAME_CODE = "%name%";     // Object name code
constexpr auto NAME_LENGTH = 6;          // Object name code length
constexpr auto SCHEMA_CODE = "%schema%"; // Object scheme code
constexpr auto SCHEMA_LENGTH = 8;        // Object scheme code length
constexpr auto ANY_TYPE_CODE = "$any$"; // Type not present in the template code

constexpr auto DEPENDENCY_LIST_NAME =
    "DependencyList.dpn"; // Name of file that will be created. Contains list of
                          // objects and their type that required to install the
                          // patch
constexpr auto OBJECT_LIST_NAME =
    "ObjectList.txt"; // Name of ObjectList for list of all objects for
                      // installation
constexpr auto INSTALL_SCRIPT_NAME_BAT =
    "Install.bat"; // Name of install script with .bat format
constexpr auto INSTALL_SCRIPT_NAME_SH =
    "Install.sh"; // Name of install script with .shell format
constexpr auto TEMPLATES_FILE_NAME =
    "Templates.ini"; // Name of file with tamplates for searching dependencyes
constexpr auto TEMP_ERROR_FILE_NAME =
    "tempError.txt"; // Name of file with errors of installation script
constexpr auto TEMP_INFO_FILE_NAME =
    "tempInfo.txt"; // Name of file with information of installation script
constexpr auto LOG_NAME = "PatchBuilder_"; // Begining of the log file name
constexpr auto LOG_FOLDER = "logs";        // Folder name for logs
constexpr auto LOG_FORMAT = ".log";        // Folder name for logs

constexpr auto BLOCK_LINE = "----------------"; // Splitter for text outing

using namespace std;
using namespace Provider; // DBProvider namespace

class PatchBuilder {
public:
  PatchBuilder() = default;
  PatchBuilder(const string &p_patch_list_name, const string &connect_args,
               const string &p_templates_name = ""); // Constructor
  ~PatchBuilder();                                   // Distructor
  void buildPatch(const string directory); // Build patch in choosen directory
  void addLog(const string message) const; // Add message in log file

private:
  string patch_list_full_name;   // Directory and name of PatchList
  string template_text = "";     // Teplate text
  string log_file_name;          // Directory and name of file for logs
  DBProvider provider;           // Ptr to provider class
  bool is_with_errors = false;   // Become true after some error
  bool is_with_warnings = false; // Become true after some warning

  DefinitionsVector
  getScriptDataVector(const ObjectsDataVector
                          &object_data_vector) const; // Getting vector of scripts,
                                                // created by DBProvider
  ObjectsDataVector
  getObjectDataVector() const; // Getting vector of objects from source database
  void createInstallPocket(const string &directory,
                           const DefinitionsVector &scripts)
      const; // Creating sql files for scripts from DefinitionsVector and
             // creating install script file
  bool isContains(const ObjectData &data,
                  const string &script_text); // Returns true, if the object was
                                              // found in the script
  ObjectsDataVector
  getPatchListVector() const; // Getting vector of objects that contains a patch
  void
  createObjectList(const DefinitionsVector &scripts,
                   const string &directory) const; // Creating of ObjectList

  static void remove(
      ObjectsDataVector &objects_first,
      const ObjectsDataVector &objects_second); // Removing elements of second
                                                // vector from first vector
  static void removeComments(
      DefinitionsVector &scripts); // Removing all commits from script text
  regex createExpression(const ObjectData &data); // Creating regular expression
                                                  // for data from params
  static string getCurrentDateTime();             // Get current date
  void showMessage(const string &message, bool is_cerr = false) const;
};
#endif
