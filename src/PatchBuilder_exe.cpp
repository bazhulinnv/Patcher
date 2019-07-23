#include <iostream>
#include <string>
#include <PatchBuilder/PatchBuilder.h>

using namespace std;

int main() 
{
	// Temp logic
	string tempDirectory = "C://Users//Timur//Documents//Temp";
	PatchBuilder patchBuilder(tempDirectory + "//PatchList.txt", "doo", "Doors", tempDirectory + "//Templates.ini");
	patchBuilder.buildPatch(tempDirectory);
}