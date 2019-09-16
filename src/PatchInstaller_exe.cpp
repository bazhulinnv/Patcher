#include "PatchInstaller/PatchInstaller.h"
#include "Shared/ParsingTools.h"
#include <direct.h>
#include <filesystem>
#include <iostream>

using namespace std;

inline bool fileExists(std::string directory) {
  std::string file_name1 = directory;
  std::string file_name2 = directory;
  file_name1 += "/Install.bat";
  file_name2 += "/Install.sh";
  return std::filesystem::exists(file_name1) ||
         std::filesystem::exists(file_name2);
}

enum ArgumentKeys { KEY_DIRECTORY, KEY_CONNECTION, KEY_COMMAND };

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

void setArgValue(vector<ArgumentData> &args, string flag,
                 const string &value) // Get argument from vector by flag
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
  std::vector<ArgumentData> args;
  args.push_back(ArgumentData("Install script full directory",
                              {"-d", "-directory"}, KEY_DIRECTORY));
  args.push_back(ArgumentData("Connection arguments", {"-cn", "-connection"},
                              KEY_CONNECTION));
  args.push_back(
      ArgumentData("Install script command", {"-cm", "-command"}, KEY_COMMAND));
  return args;
}

void printHelp(const vector<ArgumentData> &params) // Print help information
{
  std::cout << "PatchInstaller - program for checking database objects "
               "dependencies and execution install script."
            << std::endl;
  std::cout << "To run the program with the necessary parameters, use the "
               "following flags:\n\n";
  for (ArgumentData param_data : params) // Print help information
  {
    std::cout << param_data.description << endl << "flags: ";
    for (const std::string flag : param_data.flags) {
      std::cout << flag << " ";
    }
    std::cout << std::endl << std::endl;
  }
  std::cout << "Examples:" << std::endl;
  std::cout << "1)  PatchInstaller_exe.exe -d C:/Users/User/Desktop/test/ "
               "-cn 127.0.0.1:5432:Doors:doo:rs -cm install"
            << std::endl;
  std::cout
      << "2) PatchInstaller_exe.exe -directory C:/Users/User/Desktop/test/ "
         "-connection 127.0.0.1:5432:Doors:doo:rs -command check"
      << std::endl;
}

/** Implementation of work with user. Handling different cases of incorrect
 * input data. */
int main(const int argc, char *argv[]) {
  vector<ArgumentData> args = getArguments(); // Get list of arguments

  for (int arg_index = 1; arg_index < argc; arg_index++) {
    // Showing help information in this case
    if ((argc == 2) && string(argv[arg_index]) == "-help") {
      printHelp(args);
      return 0;
    } else {
      const string flag = argv[arg_index]; // Get argument by current flag
      if (arg_index < argc - 1)            // if it is not last argument
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
  try {
    PatchInstaller patchInstaller(getArgumentByKey(args, KEY_CONNECTION).value);
    // DBProvider provider(getArgumentByKey(args, KEY_CONNECTION).value);
    if (fileExists(getArgumentByKey(args, KEY_DIRECTORY).value)) {
      // Create folder for logs
      chdir(getArgumentByKey(args, KEY_DIRECTORY).value.c_str());
      mkdir("logs");
      if (getArgumentByKey(args, KEY_COMMAND).value == "install" ||
          getArgumentByKey(args, KEY_COMMAND).value == "i") {
        LoginData p(getArgumentByKey(args, KEY_CONNECTION).value);
        patchInstaller.startInstallation(p);
      } else if (getArgumentByKey(args, KEY_COMMAND).value == "check" ||
                 getArgumentByKey(args, KEY_COMMAND).value == "ch") {
        patchInstaller.checkDependencyList("DependencyList.dpn");
      } else {
        std::cout << "Enter correct command: install/check.\n";
      }
      return 0;
    }
  } catch (exception &err) {
    std::cerr << err.what() << std::endl;
    return -1;
  }

  return 0;
}
