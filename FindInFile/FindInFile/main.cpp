#include <iostream>
#include <fstream>
using namespace std;

#define UPDATE_EVERY_X_FRAMES	1000

void main(void)
{
	const char* FILE_NAME		= "VS2008.iso";
	const char* SEARCH_TERM		= "Microsoft";
	const int SizeOfStrings		= 1024;

	char StringA[SizeOfStrings];
	char StringB[SizeOfStrings];
	char StringAB[SizeOfStrings * 2];

	memset(StringA, 0, SizeOfStrings);
	memset(StringB, 0, SizeOfStrings);
	memset(StringAB, 0, SizeOfStrings * 2);

	ifstream file_stream(FILE_NAME, std::ios_base::binary);
	if (file_stream.bad() || !file_stream.good()) return;

	int CurrentFrame = 0;
	unsigned long long int BytesRead = 0;
	bool InstanceFound = false;
	while (!file_stream.eof() && !InstanceFound)
	{
		if (++CurrentFrame > UPDATE_EVERY_X_FRAMES)
		{
			CurrentFrame -= UPDATE_EVERY_X_FRAMES;
			cout << "Bytes: " << BytesRead << std::endl;
		}

		memcpy_s(StringB, SizeOfStrings, StringA, SizeOfStrings);

		file_stream.read((char*)StringA, SizeOfStrings);
		BytesRead += SizeOfStrings;

		memcpy_s(StringAB, SizeOfStrings, StringA, SizeOfStrings);
		memcpy_s(&StringAB[SizeOfStrings], SizeOfStrings, StringB, SizeOfStrings);
		
		if (strstr(StringAB, SEARCH_TERM) != 0)
		{
			cout << "Instance Found!";
			break;
		}
	}

	file_stream.close();
}