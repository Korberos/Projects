#include "XML_File.h"

#include <iostream>

void main(int argc, char *argv[])
{
	printf_s("Testing XML File... \n");
	printf_s("-------------------- \n");
	printf_s("\n");

	XML_File* Test = NULL;
	Test = XML_File::Load_XML_File( (argc > 1 ? argv[1] : "Test_XML_1.xml" ) );
	printf_s("File is XML %sVALID!   :%s  \n\nPress ENTER to exit.\n\n", (Test ? "" : "IN"), (Test ? ")" : "("));
	delete Test;

	while (!getchar()) {}
}

//  TO DO:
//    - Take in pre-tag variables as well <TagName TagVariable="Variable">TagVariable</TagName>

