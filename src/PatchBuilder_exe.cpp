#include <iostream>
#include <string>
#include <PatchBuilder/PatchBuilder.h>

using namespace std;

int main() 
{
	string tempDirectory = "C:\\Users\\Timur\\Documents\\Temp";
	PatchBuilder patchBuilder(tempDirectory);
	patchBuilder.buildPatch(tempDirectory);
}