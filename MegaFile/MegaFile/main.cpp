#include "MegaFile.h"

void main(void)
{
	MegaFile NewMegaFile;

	NewMegaFile.AddFile("TestImport1.txt");
	NewMegaFile.AddFile("TestImport2.txt");
	NewMegaFile.SaveMegaFile("MEGAFILE.meg");
	NewMegaFile.RemoveFile("TestImport1.txt");
	NewMegaFile.RemoveFile("TestImport2.txt");

	NewMegaFile.ExtractFile("MEGAFILE.meg", "TestImport2.txt", "TestExport2.txt");
	NewMegaFile.ExtractFile("MEGAFILE.meg", "TestImport1.txt", "TestExport1.txt");
}