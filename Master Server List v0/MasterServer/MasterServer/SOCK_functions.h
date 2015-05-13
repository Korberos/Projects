#pragma once

#define uint unsigned int
#define uchar unsigned char
#define ushort unsigned short

#include "SOCK_includes.h"
#include "SOCK_tools.h"
#include "SOCK_socket.h"
#include "SOCK_buffer.h"
#include "SOCK_list.h"

// Basic Tool Functions
bool	NetConnected();
uint	IPtoUINT(char* ip);
char*	UINTtoIP(uint ip);

// Winsock Start and Stop
void	Winsock_Init();
void	Winsock_Exit();

// Connections
int		tcpconnect(char* ip, int port, int mode);
int		tcplisten(int port, int max, int mode);
int		tcpaccept(int sockid, int mode);
char*	tcpip(int sockid);
bool	tcpconnected(int sockid);
int		udpconnect(int port, int mode);
bool	setnagle(int sockid, bool value);

// Miscelaneous
int		sendmessage(int sockid, char* ip, int port, int buffid);
int		receivemessage(int sockid, int len, int buffid);
int		peekmessage(int sockid, int len, int buffid);
int		setformat(int sockid, int mode, char*sep);
int		setsync(int sockid, int mode);
bool	closesock(int sockid);
int		socklasterror(int sockid);
char*	myhost();
bool	compareip(char*ip, char*mask);
void	sockexit();
void	sockstart();
char*	hostip(char*host);
int		getsocketid(int sockid);

// IP Information
char*	lastinIP();
double	lastinPort();

// Buffer Write
int		writechar(uchar val,		int buffid);
int		writeshort(short val,		int buffid);
int		writeushort(ushort val,		int buffid);
int		writeint(int val,			int buffid);
int		writeuint(uint val,			int buffid);
int		writefloat(float val,		int buffid);
int		writedouble(double val,		int buffid);
int		writestring(char* str,		int buffid);

// Buffer Read
uchar	readchar(int buffid);
short	readshort(int buffid);
ushort	readushort(int buffid);
int		readint(int buffid);
uint	readuint(int buffid);
float	readfloat(int buffid);
double	readdouble(int buffid);
char*	readstring(int buffid);

// Buffer Information
int		getpos(int pos, int buffid);
int		clearbuffer(int buffid);
int		buffsize(int buffid);
int		setpos(int pos, int buffid);
int		bytesleft(int buffid);
int		createbuffer();
bool	freebuffer(int buffid);
bool	copybuffer(int destinationid, int sourceid);
bool	copybuffer2(int destinationid, int start, int len, int sourceid);
char*	getmacaddress();
char*	md5string(char* str);
char*	md5buffer(int buffid);
bool	bufferencrypt(char* pass, int buffid);
uint	adler32(int buffid);
bool	bufferexists(int buffid);

// File Read/Write
int		fileopen(char* name, double mode);
int		fileclose(int fileid);
int		filewrite(int fileid, int buffid);
int		fileread(int fileid, int bytes, int buffid);
int		filepos(int fileid);
int		filesetpos(int fileid, int pos);
int		filesize(int fileid);

// Winsock Handler Functionality
int		AddBuffer(CBuffer* b);
int		AddSocket(CSocket* b);
int		AddFile(HANDLE b);