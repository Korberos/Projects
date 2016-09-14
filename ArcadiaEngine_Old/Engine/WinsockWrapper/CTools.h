#pragma once

#include "CBuffer.h"
#include <Iphlpapi.h>
#include <Wininet.h>

void CalculateMD5(char *buffer, int length, char *checksum);

class CTools
{
public:
	static char* getmacaddress();
	static char* md5buffer(CBuffer* buff);
	static char* md5string(char* str);
	static bool NetConnected();
	static void encryptedbuffer(CBuffer*buff, char* key);
	static bool comparematch(char* string, char* wild);
	static unsigned int adler32(CBuffer* buff);
	static HANDLE BinOpen(char* filename, int mode);
	static bool BinClose(HANDLE hwnd);
	static int BinWrite(HANDLE hwnd, CBuffer* in);
	static int BinRead(HANDLE hwnd, int size, CBuffer* out);
	static int BinGetPos(HANDLE hwnd);
	static int BinSetPos(HANDLE hwnd, int offset);
	static int BinFileSize(HANDLE hwnd);
};
