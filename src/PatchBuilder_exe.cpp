#include <iostream>
#include <string>
#include <PatchBuilder/PatchBuilder.h>

using namespace std;

int main() 
{
	// Temp logic
	string tempDirectory = "C:\\Users\\Timur\\Documents\\Temp";
	PatchBuilder patchBuilder(tempDirectory + "\\PatchList.txt");
	patchBuilder.buildPatch(tempDirectory);
}