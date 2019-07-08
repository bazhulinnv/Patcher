#include <iostream>
#include <fstream>

#include "PatchInstaller/PatchInstaller.h"

using namespace std;

int main() { 
	//std::cout << "hello world" << std::endl; 
	std::string nameOfFile =  "dependencies.txt";
	std::cout << "We are here!" << "\n";
	PatchInstaller patchInstaller;
	std::cout<< patchInstaller.checkObjectsForExistence(nameOfFile);
	/*string str;
	std::ofstream f("1.txt");
	f << "The first string" << '\n';
	cin >> str;
	std::ofstream("1.txt", ios::app);
	f << str;
	f.close();*/
}
