#include "PatchBuilder/PatchBuilder.h"
#include <cstdio>
#include <ctime>
#include <direct.h>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <streambuf>

using namespace std;

PatchBuilder::PatchBuilder(const string &value_patch_list_name,
                           const string &connect_args,
                           const string &p_templates_name) {
  patch_list_full_name = value_patch_list_name;
  provider = DBProvider(string(connect_args));
  // Check tamplate file
  ifstream input;
  input.open(p_templates_name);
  if (input.is_open()) {
    template_text =
        string((istreambuf_iterator<char>(input)),
               istreambuf_iterator<char>()); // Reading all text file in string
    input.close();
  } else {
    input.open(TEMPLATES_FILE_NAME);
    if (!input.is_open()) {
      showMessage(
          "Cannot open or find template file\nDependencyList will be formed by "
          "presence of names\n");
      is_with_warnings = true;
    } else {
      template_text = string(
          (istreambuf_iterator<char>(input)),
          istreambuf_iterator<char>()); // Reading all text file in string
      input.close();
    }
  }
}
PatchBuilder::~PatchBuilder() {}

void PatchBuilder::buildPatch(const string directory) {
  if (!filesystem::exists(directory)) {
    throw exception("Incorrect patch directory");
  }

  // Creating log file
  string log_directory = (directory + "/" + LOG_FOLDER);
  _mkdir(&log_directory[0]);
  log_file_name =
      log_directory + "/" + LOG_NAME + getCurrentDateTime() + LOG_FORMAT;

  // Executing all methods for patch building
  ofstream output(directory + "/" +
                  DEPENDENCY_LIST_NAME); // Dependency list directory
<<<<<<< Updated upstream
  const objectDataVector patch_objects =
      getPatchListVector(); // Getting vector that contains all patch objects
  scriptDataVector scripts = getScriptDataVector(
=======
  const ObjectsDataVector patch_objects =
      getPatchListVector(); // Getting vector that contains all patch objects
  DefinitionsVector scripts = getScriptDataVector(
>>>>>>> Stashed changes
      patch_objects); // Getting all scripts created by DBProvider
  createObjectList(scripts, directory);    // Creating of ObjectList
  createInstallPocket(directory, scripts); // Creaing all instalation components
  removeComments(scripts);
<<<<<<< Updated upstream
  objectDataVector objects =
      getObjectDataVector();      // Getting vector that contains all objects of
                                  // source databse
  remove(objects, patch_objects); // Removing path objects from objectDataVector
=======
  ObjectsDataVector objects =
      getObjectDataVector(); // Getting vector that contains all objects of
                             // source databse
  remove(objects,
         patch_objects); // Removing path objects from ObjectsDataVector
>>>>>>> Stashed changes

  // Writing of DependencyList
  showMessage(string("Parsing started...\n") + BLOCK_LINE + "\n");
  for (const ObjectData object : objects) {
    // Checking all objects for the presence in scripts
    for (const ScriptData script : scripts) {
      if (isContains(object, script.text)) {
        // If object was found - writing it's name and type in DependencyList
        output << object.schema << " " << object.name << " " << object.type
               << endl;
        showMessage(" - " + object.type + " " + object.name +
                    " included - dependency in " + script.name + "\n");
        break;
      }
    }
  }
  output.close();
  showMessage((BLOCK_LINE + string("\n")));

  string message;
  if (is_with_errors) {
    message = "Patch built with errors!\n";
  } else {
    if (is_with_warnings) {
      message = "Patch built with warnings!\n";
    } else {
      message = "Patch built successfully!\n";
    }
  }
  showMessage(message);
}

<<<<<<< Updated upstream
scriptDataVector
PatchBuilder::getScriptDataVector(const objectDataVector &objects) {
  // Not implemented
  scriptDataVector scripts;
=======
DefinitionsVector
PatchBuilder::getScriptDataVector(const ObjectsDataVector &objects) {
  // Not implemented
  DefinitionsVector scripts;
>>>>>>> Stashed changes
  for (ObjectData object : objects) {
    if (object.schema == "script") {
      // Reading all text from file
      ifstream input(object.name);
      const string text((istreambuf_iterator<char>(input)),
                        istreambuf_iterator<char>());

      // Remove path to file leave only name
      const size_t slash_pos = object.name.find_last_of("/");
      object.name.erase(0, slash_pos + 1);

      // Add script in vector
      const ScriptData script_data(object, text);
      scripts.push_back(script_data);

      input.close();
    } else {
      // Getting script data from DBProvider
      vector<ScriptData> extra_scripts; // Vector for
      const ScriptData script = provider.getScriptData(object, extra_scripts);
      scripts.push_back(script);

      if (!extra_scripts.empty()) {
        for (const ScriptData &script : extra_scripts) {
          scripts.push_back(script);
        }
      }
    }
  }
  showMessage("Script vector created\n");
  return scripts;
}

<<<<<<< Updated upstream
objectDataVector PatchBuilder::getObjectDataVector() {
  // Getting all source database objects
  objectDataVector objects = provider.getObjects();
=======
ObjectsDataVector PatchBuilder::getObjectDataVector() {
  // Getting all source database objects
  ObjectsDataVector objects = provider.GetObjects();
>>>>>>> Stashed changes

  showMessage("Object vector created\n");
  return objects;
}

void PatchBuilder::createInstallPocket(const string &directory,
<<<<<<< Updated upstream
                                       const scriptDataVector &scripts) const {
=======
                                       const DefinitionsVector &scripts) const {
>>>>>>> Stashed changes
  ofstream output_bat(directory + "/" + INSTALL_SCRIPT_NAME_BAT);
  ofstream output_sh(directory + "/" + INSTALL_SCRIPT_NAME_SH);

  // Files to logging of install script working
  vector<string> temp_names;
  temp_names.push_back(TEMP_ERROR_FILE_NAME);
  temp_names.push_back(TEMP_INFO_FILE_NAME);

  string out_operator = ">";

  // Creating sql files for all scrpits and writing install script
  for (const ScriptData &data : scripts) {
    // Creating directory named as type of script
    _mkdir(&(directory + "/" + data.schema)[0]);
    _mkdir(&(directory + "/" + data.schema + "/" + data.type)[0]);
    ofstream output_script(directory + "/" + data.schema + "/" + data.type +
                           "/" + data.name);
    // Writing script text in file
    output_script << data.text;
    output_script.close();

    // Creating install command
    string install_command = string("psql -a -U ") + "%1" + " -d " + "%2" +
                             " -h " + "%3" + " -p " +
                             "%4"
                             " -f " +
                             data.schema + "/";
    if (!data.type.empty()) {
      install_command += data.type + "/";
    }
    install_command += data.name + " ";

    install_command += to_string(temp_names.size());
    for (const string &name : temp_names) {
      install_command += out_operator + name;
    }
    install_command += "\n";

    output_bat << install_command; // Writing psql command in InstallScript with
                                   // .bat format
    output_sh << install_command;  // Writing psql command in InstallScript with
                                   // .sh format

    // Should use > operator to create file and write in it
    // Should use >> operator to write in existing file
    if (out_operator.size() == 1) {
      out_operator += ">";
    }
  }

  output_bat.close();
  output_sh.close();

  showMessage("Install pocket created\n");
}

bool PatchBuilder::isContains(const ObjectData &data,
                              const string &script_text) {
  cmatch result; // To contain result of searching

  // If template is empty - searching by object name
  if (template_text.empty()) {
    return (regex_search(script_text.c_str(), result, regex(data.name)));
  }

  // Checking on the content of the object in current script
  const regex reg_ex = createExpression(data); // Creating regular expression
  try {
    return (regex_search(script_text.c_str(), result, reg_ex));
  } catch (exception &err) {
    // If can not searching regular expression
    string message = "WARNING - can not search regular expression from "
                     "template:\nDESCRIPTION - ";
    message += err.what();
    message += "\n";
    message += "returning false result for " + data.name + "\n";
    showMessage(message, true);
    is_with_warnings = true;
    return false;
  }
}

<<<<<<< Updated upstream
objectDataVector PatchBuilder::getPatchListVector() const {
  // Getting all patch objects
  objectDataVector patch_objects;
=======
ObjectsDataVector PatchBuilder::getPatchListVector() const {
  // Getting all patch objects
  ObjectsDataVector patch_objects;
>>>>>>> Stashed changes
  ifstream input(patch_list_full_name);
  if (input.is_open()) {
    while (!input.eof()) {
      // Reading from PatchList file in patchListVector
      ObjectData object;
      input >> object.schema;

      // If end of file
      if (object.schema.empty()) {
        return patch_objects;
      }

      // If this is script from outside - type field is empty
      if (object.schema == "script") {
        input >> object.name;
        object.type = "";
      } else {
        input >> object.name;
        input >> object.type;
      }

      // If type is "function" reading params of it
      if (object.type == "function") {
        string current_word;
        input >> current_word;
        input >> current_word;
        while (current_word != ")") {
          object.params.push_back(current_word);
          input >> current_word;
        }
      }
      patch_objects.push_back(object);
    }

    input.close();
    return patch_objects;
  } else {
    throw exception("Cannot open patch list");
  }
}

<<<<<<< Updated upstream
void PatchBuilder::createObjectList(const scriptDataVector &objects,
=======
void PatchBuilder::createObjectList(const DefinitionsVector &objects,
>>>>>>> Stashed changes
                                    const string &directory) const {
  ofstream output(directory + "/" + OBJECT_LIST_NAME);
  for (ObjectData data : objects) {
    output << data.schema << " " << data.name << " " << data.type;
    if (data.type == "function") {
      output << " ( ";
      for (string param : data.params) {
        output << param << " ";
      }
      output << ")";
    }
    output << endl;
  }
  output.close();
}

<<<<<<< Updated upstream
void PatchBuilder::remove(objectDataVector &objects_first,
                          const objectDataVector &objects_second) {
=======
void PatchBuilder::remove(ObjectsDataVector &objects_first,
                          const ObjectsDataVector &objects_second) {
>>>>>>> Stashed changes
  // Removing elements of second vector from first vector
  for (size_t index = 0; index < objects_first.size(); index++) {
    for (const ObjectData &object : objects_second) {
      if (object == objects_first[index]) {
        objects_first.erase(objects_first.begin() + index);
        index--;
        break;
      }
    }
  }
}

<<<<<<< Updated upstream
void PatchBuilder::removeComments(scriptDataVector &scripts) {
=======
void PatchBuilder::removeComments(DefinitionsVector &scripts) {
>>>>>>> Stashed changes
  // Removing of all commits
  for (ScriptData &script : scripts) {
    // "--" comments removing
    string &text = script.text;
    size_t start = text.find("--"); // Find "--" in text
    size_t end;
    while (start != string::npos) {
      // While can not to find "--" in text
      end = text.find(
          "\n", start); // Find "new line symbol" in text after position of "--"
      text.erase(start, end - start + 1); // Remove this part of text
      start = text.find("--");            // Try to find next "--"
    }

    // "/* */" comments removing
    start = text.find("/*"); // Find "/*" in text
    while (start != string::npos) {
      // While can not to find "/*" in text
      end = text.find("*/", start); // Find "*/" in text after position of "/*"
      text.erase(start, end - start + 2); // Remove this part of text
      start = text.find("/*");            // Try to find next "/*"
    }
  }
}

regex PatchBuilder::createExpression(const ObjectData &data) {
  // Determine the type of the object and use the appropriate template
  string current_word = "";
  string reg_ex = "";
  stringstream template_stream;
  template_stream << template_text;
  while (!template_stream.eof()) {
    template_stream >> current_word;
    // if have found type code word
    if (current_word == TYPE_CODE) {
      template_stream >> current_word;
      template_stream >> current_word;
      // Compare current template type with object type
      if (current_word == data.type || current_word == ANY_TYPE_CODE) {
        // If types are equal or it template for any type
        template_stream >> current_word;
        template_stream >> current_word;
        // Reading all reglular expressions
        // until the end code word is found
        while (current_word != END_CODE) {
          // Replace name code word on current object name
          const size_t namePos = current_word.find(NAME_CODE);
          if (namePos != string::npos) {
            current_word.replace(namePos, NAME_LENGTH, data.name);
          }

          // Replace scheme code word on current object schema
          const size_t schema_pos = current_word.find(SCHEMA_CODE);
          if (schema_pos != string::npos) {
            current_word.replace(schema_pos, SCHEMA_LENGTH, data.schema);
          }

          // Replace scheme code word on current object schema
          const size_t endl_pos = current_word.find("\n");
          if (endl_pos != string::npos) {
            current_word.pop_back();
            current_word.pop_back();
          }

          // Concatenate this regular expressions with other
          reg_ex += current_word;
          reg_ex += "|";

          // Reading next
          template_stream >> current_word;
        }

        // Remove last "|" symbol
        reg_ex.pop_back();
        try {
          return regex(reg_ex);
        } catch (exception &err) {
          // If cannot create regular expression
          string message = "WARNING - can not create regular expression from "
                           "template:\nDESCRIPTION - ";
          message += err.what();
          message += "\n";
          message += "returning simple expression by object name for " +
                     data.name + "\n";
          showMessage(message, true);
          is_with_warnings = true;
          return regex(data.name);
        }
      }
    }
  }

  throw exception(
      ("Cannot create regular expression for " + data.name).c_str());
}

string PatchBuilder::getCurrentDateTime() {
  // Getting current date
  time_t now = time(0);
  struct tm tstruct;
  char buf[80];
  tstruct = *localtime(&now);
  strftime(
      buf, sizeof(buf), "%F-%H-%M-%S",
      &tstruct); // Returning time in "year-month-day-hour-minute-second" format
  return string(buf);
}

void PatchBuilder::showMessage(const string &message, bool is_cerr) const {
  if (is_cerr) {
    cerr << message;
  } else {
    cout << message;
  }
  addLog(message);
}

void PatchBuilder::addLog(const string message) const {
  // Writing message in log file
  ofstream output(log_file_name, std::ios_base::app);
  output << message;
  output.close();
}
