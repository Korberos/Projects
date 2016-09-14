#pragma once

#include <Winsock2.h>
#include <Assert.h>
#include <stdio.h>

class CBuffer
{
	static char retval[20001];
public:
	char* data;
	int BuffSize;
	int readpos;
	int writepos;
	void StreamWrite(void *in, int size);
	void StreamRead(void* out, int size, bool peek);
	int count;
	CBuffer();
	~CBuffer();
	int 				writechar(unsigned char a);
	int 				writeshort(short a);
	int 				writeushort(unsigned short a);
	int 				writeint(int a);
	int 				writeuint(unsigned int a);
	int 				writefloat(float a);
	int 				writedouble(double a);
	int 				writechars(char*str);
	int 				writechars(const char*str);
	int 				writestring(char*str);
	int 				writestring(const char*str);

	unsigned char	readchar( bool peek = false );
	short				readshort( bool peek = false );
	unsigned short	readushort( bool peek = false );
	int				readint( bool peek = false );
	unsigned int	readuint( bool peek = false );
	float				readfloat( bool peek = false );
	double			readdouble( bool peek = false );
	char*				readchars( int len, bool peek = false );
	char*				readstring( bool peek = false );

	int bytesleft();
	void StreamSet(int pos);
	void clear();
	int addBuffer(char*, int);
	int addBuffer(CBuffer*);
	char operator[](int index);
};
