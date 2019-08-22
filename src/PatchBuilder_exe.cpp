#include "DBProvider/DBProvider.h"
#include "PatchBuilder/PatchBuilder.h"
#include <ctype.h>
#include <iostream>
#include <optional>
#include <string>

using namespace std;

// Key values for ArgumentData
enum ArgumentKeys { KEY_DIRECTORY, KEY_CONNECTION, KEY_PATH, KEY_TEMPLATE };

// Structure for input arguments information
struct ArgumentData {
  ArgumentData() : key(), is_required(false) {}

  ArgumentData(const string p_desciprion, const vector<string> p_flags,
               const ArgumentKeys p_key, const bool p_is_required = true) {
    description = p_desciprion;
    flags = p_flags;
    key = p_key;
    is_required = p_is_required;
  }
  string description;   // description for help information
  vector<string> flags; // all flags for argument
  ArgumentKeys key;     // unic key
  string value;         // value of argument
  bool is_required;     // is required to build patch
};

void setArgValue(vector<ArgumentData> &args,
             string flag, const string &value) // Get argument from vector by flag
{
  transform(flag.begin(), flag.end(), flag.begin(),
            tolower); // Transform to lower register
  // Looking for a match with the flag
  for (ArgumentData &arg : args) {
    for (const string &current_flag : arg.flags) {
      if (current_flag == flag) {
        arg.value = value;
      }
    }
  }
}

ArgumentData &
getArgumentByKey(vector<ArgumentData> &args,
                 const ArgumentKeys &key) // Get argument from vector by key
{
  // Looking for a match with the key
  for (ArgumentData &arg : args) {
    if (arg.key == key) {
      return arg; // Return the argument with the right key
    }
  }

  throw(exception("Wrong argument key"));
}

vector<ArgumentData>
getArguments() // Init all args data and return list of their
{
  vector<ArgumentData> args;
  args.push_back(
      ArgumentData("PatchList full path", {"-p", "-patch"}, KEY_PATH));
  args.push_back(ArgumentData("Patch building directory", {"-d", "-directory"},
                              KEY_DIRECTORY));
  args.push_back(ArgumentData("Connection arguments", {"-c", "-connection"},
                              KEY_CONNECTION));
  args.push_back(ArgumentData("Templates file full path [optional]",
                              {"-t", "-template"}, KEY_TEMPLATE, false));
  return args;
}

void printHelp(const vector<ArgumentData> &params) // Print help information
{
  cout << "PatchBuilder - program for patch building by list of objects\n"
       << endl;
  cout << "To run the program with the necessary parameters, use the following "
          "flags:\n\n";
  for (ArgumentData param_data : params) // Print help information
  {
    cout << param_data.description << endl << "flags: ";
    for (const string flag : param_data.flags) {
      cout << flag << " ";
    }
    cout << endl << endl;
  }
  cout << "Examples:" << endl;
  cout << "1) -d C:\\PatchDirectory -p C:\\PatchList.txt   -c "
          "127.0.0.1:5432:database:user:password   -t C:\\Templates.ini"
       << endl;
  cout << "2) -connect 127.0.0.1:5432:database:user:password   -patch "
          "C:\\PatchList.txt   -directory C:\\PatchDirectory"
       << endl;
}

int main(const int argc, char *argv[]) {
  vector<ArgumentData> args = getArguments(); // Get list of arguments

  for (int arg_index = 1; arg_index < argc; arg_index++) {
    // Showing help information in this case
    if ((argc == 2) && string(argv[arg_index]) == "-help") {
      printHelp(args);
      return 0;
    } else {
      const string flag = argv[arg_index]; // Get argument by current flag
      if (arg_index < argc - 1) // if it is not last argument
      {
        arg_index++;
        const string value = string(argv[arg_index]);
        setArgValue(args, flag, value);
      }
    }
  }

  // Checking on emptiness of required arguments values
  for (ArgumentData &arg : args) {
    if (arg.value.empty() && arg.is_required) {
      cout << "You must specify the following argument:\n" << arg.description;
      return -1;
    }
  }

  // If everything is fine with the arguments try to build path
  PatchBuilder *builder;
  try {
    const string template_path = getArgumentByKey(args, KEY_TEMPLATE).value;
    const string connect_args = getArgumentByKey(args, KEY_CONNECTION).value;
    const string patch_list_path = getArgumentByKey(args, KEY_PATH).value;
    const string patch_directory = getArgumentByKey(args, KEY_DIRECTORY).value;
    builder = new PatchBuilder(patch_list_path, connect_args, template_path);
    builder->buildPatch(patch_directory);
    return 0;
  } catch (exception &err) {
    cerr << err.what() << endl;
    builder->addLog(err.what());
    return -1;
  }

  return 0;
}