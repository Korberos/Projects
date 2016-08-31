#include "CBuffer.h"

#define SIZEOF_CHAR sizeof(char)
#define SIZEOF_SHRT sizeof(short)
#define SIZEOF_USRT sizeof(unsigned short)
#define SIZEOF_INTE sizeof(int)
#define SIZEOF_UINT sizeof(unsigned int)
#define SIZEOF_FLOT sizeof(float)
#define SIZEOF_DOUB sizeof(double)

char CBuffer::retval[20001];
CBuffer::CBuffer()
{
	BuffSize = 30;
	data = (char*)malloc(BuffSize);
	count = 0;
	readpos = 0;
	writepos = 0;
}

CBuffer::~CBuffer()
{
	if (data != NULL) free(data);
}

void CBuffer::StreamWrite(void *in, int size)
{
	if(writepos+size >= BuffSize)
	{
		BuffSize = writepos+size + 30;
		if ((data = (char*)realloc(data, BuffSize)) == NULL) return;
	}
	memcpy(data+writepos, in, size);
	writepos += size;
	if (writepos > count) count = writepos;
}

void CBuffer::StreamRead(void* out, int size, bool peek)
{
	if (readpos + size > count) size = count - readpos;
	if (size <= 0) return;
	memcpy(out, data + readpos, size);

	if ( !peek )
	{
		readpos += size;
	}
}

int CBuffer::writechar(unsigned char val)
{
	StreamWrite(&val, 1);
	return SIZEOF_CHAR;
}

int CBuffer::writeshort(short a)
{
	StreamWrite(&a, 2);
	return SIZEOF_SHRT;
}

int CBuffer::writeushort(unsigned short a)
{
	StreamWrite(&a, 2);
	return SIZEOF_USRT;
}
int CBuffer::writeint(int a)
{
	StreamWrite(&a, 4);
	return SIZEOF_INTE;
}
int CBuffer::writeuint(unsigned int a)
{
	StreamWrite(&a, 4);
	return SIZEOF_UINT;
}

int CBuffer::writefloat(float a)
{
	StreamWrite(&a, 4);
	return SIZEOF_FLOT;
}
int CBuffer::writedouble(double a)
{
	StreamWrite(&a, 8);
	return SIZEOF_DOUB;
}

int CBuffer::writechars(char*str)
{
	int len = (int)strlen(str);
	StreamWrite(str, len);
	return len;
}

int CBuffer::writechars(const char*str)
{
	int len = (int)strlen(str);
	StreamWrite( (void*)(str), len);
	return len;
}

int CBuffer::writestring(char *str)
{
	int len = writechars(str);
	return len + writechar('\0');
}

int CBuffer::writestring(const char *str)
{
	int len = writechars(str);
	return len + writechar('\0');
}

unsigned char CBuffer::readchar( bool peek )
{
	unsigned char b;
	StreamRead(&b, 1, peek);
	return b;
}

short CBuffer::readshort( bool peek )
{
	short b;
	StreamRead(&b, 2, peek);
	return b;
}

unsigned short CBuffer::readushort( bool peek )
{
	unsigned short b;
	StreamRead(&b, 2, peek);
	return b;
}

int CBuffer::readint( bool peek )
{
	int b;
	StreamRead(&b, 4, peek);
	return b;
}

unsigned int CBuffer::readuint( bool peek )
{
	unsigned int b;
	StreamRead(&b, 4, peek);
	return b;
}

float CBuffer::readfloat( bool peek )
{
	float b;
	StreamRead(&b, 4, peek);
	return b;
}
double CBuffer::readdouble( bool peek )
{
	double b;
	StreamRead(&b, 8, peek);
	return b;
}

char* CBuffer::readchars( int len, bool peek )
{
	
	if(len < 0)return NULL;
	StreamRead(&retval, len, peek);
	retval[len] = '\0';
	return retval;
}

char* CBuffer::readstring( bool peek )
{
	int i;
	for(i = readpos; i < count; i++)
		if (data[i] == '\0') break;

	if (i == count) return NULL;
	i -= readpos;
	i = min(20000, i);
	StreamRead(&retval, i + 1, peek);
	return retval;
}

int CBuffer::bytesleft()
{
	return count - readpos;
}

int CBuffer::addBuffer(CBuffer *buffer)
{
	StreamWrite(buffer->data, buffer->count);
	return buffer->count;
}

int CBuffer::addBuffer(char *data, int len)
{
	StreamWrite(data, len);
	return len;
}

void CBuffer::clear()
{
	if(BuffSize > 30)
	{
		free(data);
		BuffSize = 30;
		data =(char*) malloc(BuffSize);
	}
	count = 0;
	readpos = 0;
	writepos = 0;
}

void CBuffer::StreamSet(int pos)
{
	readpos = 0;
	writepos = 0;
}

char CBuffer::operator [](int i)
{
	return ((i < 0 || i >= count) ? '\0' : data[i]);
}