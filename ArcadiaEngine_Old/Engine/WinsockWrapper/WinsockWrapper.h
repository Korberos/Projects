#pragma once

#include "CSocket.h"

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Iphlpapi.lib")
#pragma comment(lib, "Wininet.lib")

// Basic Tool Functions
bool			NetConnected();
unsigned int	IPtoUINT(char* ip);
char*			UINTtoIP(unsigned int ip);

// Winsock Start and Stop
void			Winsock_Init( unsigned int buffer_count = 1 );
void			Winsock_Exit();

// Connections
int				tcpconnect(char* ip, int port, int mode);
int				tcplisten(int port, int max, int mode);
int				tcpaccept(int sockid, int mode);
char*			tcpip(int sockid);
bool			tcpconnected(int sockid);
int				udpconnect(int port, int mode);
bool			setnagle(int sockid, bool value);

// Miscelaneous
int				sendmessage(int sockid, const char* ip, int port, int buffid);
int				receivemessage( int sockid, int len, int buffid, int length_specific = 0 );
int				peekmessage(int sockid, int len, int buffid);
int				setformat(int sockid, int mode, char* sep);
int				setsync(int sockid, int mode);
bool			closesock(int sockid);
int				socklasterror(int sockid);
char*			myhost();
bool			compareip(char* ip, char* mask);
void			sockexit();
void			sockstart();
char*			hostip(char* host);
int				getsocketid(int sockid);

// IP Information
char*			lastinIP();
double			lastinPort();

// Buffer Write
int				writechar(unsigned char val,	int buffid);
int				writeshort(short val,			int buffid);
int				writeushort(unsigned short val,	int buffid);
int				writeint(int val,				int buffid);
int				writeuint(unsigned int val,		int buffid);
int				writefloat(float val,			int buffid);
int				writedouble(double val,			int buffid);
int				writestring(char* str,			int buffid);
int				writestring(const char* str,	int buffid);

// Buffer Read
unsigned char	readchar( int buffid, bool peek = false );
short			readshort( int buffid, bool peek = false );
unsigned short	readushort( int buffid, bool peek = false );
int				readint( int buffid, bool peek = false );
unsigned int	readuint( int buffid, bool peek = false );
float			readfloat( int buffid, bool peek = false );
double			readdouble( int buffid, bool peek = false );
char*			readstring( int buffid, bool peek = false );

// Buffer Information
int				getpos( bool readwrite, int buffid );
int				clearbuffer(int buffid);
int				buffsize(int buffid);
int				setpos(int pos, int buffid);
int				bytesleft(int buffid);
int				createbuffer();
bool			freebuffer(int buffid);
bool			copybuffer(int destinationid, int sourceid);
bool			copybuffer2(int destinationid, int start, int len, int sourceid);
char*			getmacaddress();
char*			md5string(char* str);
char*			md5buffer(int buffid);
bool			bufferencrypt(char* pass, int buffid);
unsigned int	adler32(int buffid);
bool			bufferexists(int buffid);

// File Read/Write
int				fileopen(char* name, double mode);
int				fileclose(int fileid);
int				filewrite(int fileid, int buffid);
int				fileread(int fileid, int bytes, int buffid);
int				filepos(int fileid);
int				filesetpos(int fileid, int pos);
int				filesize(int fileid);

// Winsock Handler Functionality
int				AddBuffer(CBuffer* b);
int				AddSocket(CSocket* b);
int				AddFile(HANDLE b);