#include "WinsockWrapper.h"
#include "CTools.h"
#include <vector>

std::vector< CBuffer* > buffers;
std::vector< CSocket* > sockets;
std::vector< HANDLE >  files;

/*****************************************************************/
//	Function:	NetConnected
//	Purpose:	Returns whether you are currently connected to the internet
/*****************************************************************/
bool NetConnected()
{
	return CTools::NetConnected();
}

/*****************************************************************/
//	Function:	IPtoUINT
//	Purpose:	Returns the IP given in the form of an unsigned int
/*****************************************************************/
unsigned int IPtoUINT(char* ip)
{
	return inet_addr(ip);
}

/*****************************************************************/
//	Function:	UINTtoIP
//	Purpose:	Returns the unsigned int given in the form of an IP
/*****************************************************************/
char* UINTtoIP(unsigned int ip)
{
	IN_ADDR a;
	a.s_addr = (u_long)ip;
	return inet_ntoa(a);
}

/*****************************************************************/
//	Function:	Winsock_Init
//	Purpose:	Initializes the Winsock wrapper functionality
/*****************************************************************/
void Winsock_Init( unsigned int buffer_count )
{
    CSocket::SockStart();

	for ( unsigned int i = 0; i < buffer_count; ++i )
	{
		AddBuffer( new CBuffer() );
	}

	AddSocket( new CSocket() );
}

/*****************************************************************/
//	Function:	Winsock_Exit
//	Purpose:	Cleans up the buffer, socket, and file lists
/*****************************************************************/
void Winsock_Exit()
{
	CSocket::SockExit();

	for ( int i = 0; i < int(buffers.size()); ++i )
	{
		delete buffers[i];
	}

	for ( int i = 0; i < int(sockets.size()); ++i )
	{
		delete sockets[i];
	}

	for ( int i = 0; i < int(files.size()); ++i )
	{
		CTools::BinClose(files[i]);
	}

	buffers.clear();
	sockets.clear();
	files.clear();
}

/*****************************************************************/
//	Function:	tcpconnect
//	Purpose:	Connect to a TCP-based server given the contact information
/*****************************************************************/
int tcpconnect(char* ip, int port, int mode)
{
	CSocket* sock = new CSocket();
	if(sock->tcpconnect(ip, port, mode)) return AddSocket(sock);

	delete sock;
	return -1;
}

/*****************************************************************/
//	Function:	tcplisten
//	Purpose:	Creates a listening server given contact information
/*****************************************************************/
int tcplisten(int port, int max, int mode)
{
	CSocket* sock = new CSocket();
	if (sock->tcplisten(port, max, mode)) return AddSocket(sock);

	delete sock;
	return -1;
}

/*****************************************************************/
//	Function:	tcpaccept
//	Purpose:	Checks for a joining connection and returns the ID if applicable
/*****************************************************************/
int tcpaccept(int sockid, int mode)
{
	CSocket* sock1	= sockets[sockid];
	if (sock1 == NULL)	return -1;

	CSocket* sock2	= sock1->tcpaccept(mode);
	return ((sock2 != NULL) ? AddSocket(sock2) : -1);
}

/*****************************************************************/
//	Function:	tcpip
//	Purpose:	Returns the IP that the socket is connected to
/*****************************************************************/
char* tcpip(int sockid)
{
	CSocket* sock = sockets[sockid];
	return ((sock == NULL) ? 0 : sock->tcpip());
}

/*****************************************************************/
//	Function:	tcpconnected
//	Purpose:	Given a socket connection identifier, returns whether you are connected
/*****************************************************************/
bool tcpconnected(int sockid)
{
	CSocket* sock = sockets[sockid];
	return ((sock == NULL) ? true : false);
}

/*****************************************************************/
//	Function:	udpconnect
//	Purpose:	Connects to a UDP server given the contact information
/*****************************************************************/
int udpconnect(int port, int mode)
{
	CSocket* sock = new CSocket();
	if (sock->udpconnect(port, mode)) return AddSocket(sock);

	delete sock;
	return -1;
}

/*****************************************************************/
//	Function:	setnagle
//	Purpose:	Toggles the buffer delay algorithm (Naggle)
/*****************************************************************/
bool setnagle(int sockid, bool value)
{
	CSocket* sock = sockets[sockid];
	if (sock == NULL)	return false;
	if (value)	sock->setnagle(true);
	else		sock->setnagle(false);
	return true;
}

/*****************************************************************/
//	Function:	sendmessage
//	Purpose:	Sends the current write buffer over the socket connection specified
/*****************************************************************/
int sendmessage(int sockid, const char* ip, int port, int buffid)
{
	CSocket* sock = sockets[sockid];
	CBuffer* buff = buffers[buffid];
	if (sock == NULL) return -1;
	if (buff == NULL) return -2;
	int size = sock->sendmessage(ip, port, buff);
	if (size < 0) return -sock->lasterror();
	return size;
}

/*****************************************************************/
//	Function:	receivemessage
//	Purpose:	Receives the top message in the message buffer and removes it
/*****************************************************************/
int receivemessage(int sockid, int len, int buffid, int length_specific)
{
	CSocket* sock = sockets[sockid];
	CBuffer* buff = buffers[buffid];
	if (sock == NULL) return -1;
	if (buff == NULL) return -2;
	int size = sock->receivemessage( len, buff, length_specific );
	if (size < 0)
	{
		int error = sock->lasterror();
		if (error == 10054) return 0;
		return -error;
	}
	return size;
}

/*****************************************************************/
//	Function:	peekmessage
//	Purpose:	Receives the top message in the message buffer
/*****************************************************************/
int peekmessage(int sockid, int len, int buffid)
{
	CSocket* sock = sockets[sockid];
	CBuffer* buff = buffers[buffid];
	if (sock == NULL) return -1;
	if (buff == NULL) return -2;
	int size = sock->peekmessage(len, buff);
	if (size < 0)
	{
		int error = sock->lasterror();
		return ((error == 10054) ? 0 : -error);
	}
	return size;
}

/*****************************************************************/
//	Function:	setformat
//	Purpose:	Alters message sending format (0 = binary, 1 = text, 2 = none)
/*****************************************************************/
int setformat(int sockid, int mode, char* sep)
{
	CSocket* sock = sockets[sockid];
	return ((sock == NULL) ? -1 : sock->SetFormat(mode, sep));
}

/*****************************************************************/
//	Function:	setsync
//	Purpose:	Sets the Blocking/Non-Blocking variable for the connection
/*****************************************************************/
int setsync(int sockid, int mode)
{
	if ( sockid < 0 ) return -1;
	CSocket* sock = sockets[sockid];
	if (sock == NULL) return -1;
	sock->setsync(mode);
	return 1;
}

/*****************************************************************/
//	Function:	closesock
//	Purpose:	Closes the given socket connection and cleans up extra information
/*****************************************************************/
bool closesock(int sockid)
{
	if ( sockid < 0 ) return false;
	CSocket* sock = sockets[sockid];
	if (sock == NULL) return false;
	delete sock;
	sockets[sockid] = NULL;
	return true;
}

/*****************************************************************/
//	Function:	socklasterror
//	Purpose:	Returns the last error sent from Winsock
/*****************************************************************/
int socklasterror(int sockid)
{
	CSocket* sock = sockets[sockid];
	return ((sock == NULL) ? -1 : -sock->lasterror());
}

/*****************************************************************/
//	Function:	myhost
//	Purpose:	Returns the host name
/*****************************************************************/
char* myhost()
{
	return CSocket::myhost();
}

/*****************************************************************/
//	Function:	compareip
//	Purpose:	Compares an IP to a masked IP (* for masked numbers)
/*****************************************************************/
bool compareip(char* ip, char* mask)
{
	return CTools::comparematch(ip, mask);
}

/*****************************************************************/
//	Function:	sockexit
//	Purpose:	Finishes up closing the socket handling functionality in Winsock
/*****************************************************************/
void sockexit()
{
	CSocket::SockExit();
}

/*****************************************************************/
//	Function:	sockstart
//	Purpose:	Begins the socket handling functionality in Winsock
/*****************************************************************/
void sockstart()
{
	CSocket::SockStart();
}

/*****************************************************************/
//	Function:	hostip
//	Purpose:	Returns the IP given the host name
/*****************************************************************/
char* hostip(char* host)
{
	return CSocket::GetIp(host);
}

/*****************************************************************/
//	Function:	getsocketid
//	Purpose:	Returns the WINSOCK ID given the Socket ID
/*****************************************************************/
int getsocketid(int sockid)
{
	CSocket* sock = sockets[sockid];
	return ((sock == NULL) ? -1 : (int)sock->sockid);
}

/*****************************************************************/
//	Function:	lastinIP
//	Purpose:	Returns the IP of the last entered connection
/*****************************************************************/
char* lastinIP()
{
	return CSocket::lastinIP();
}

/*****************************************************************/
//	Function:	lastinPort
//	Purpose:	Returns the Port of the last entered connection
/*****************************************************************/
double lastinPort()
{
	return CSocket::lastinPort();
}

/*****************************************************************/
//	Function:	writechar
//	Purpose:	Writes a char (1 byte) to the message buffer
/*****************************************************************/
int writechar(unsigned char val, int buffid)
{
	CBuffer* buff = buffers[buffid];
	return ((buff == NULL) ? 0 : buff->writechar(val));
}

/*****************************************************************/
//	Function:	writeshort
//	Purpose:	Writes a short (2 bytes) to the message buffer
/*****************************************************************/
int writeshort(short val, int buffid)
{
	CBuffer* buff = buffers[buffid];
	return ((buff == NULL) ? 0 : buff->writeshort(val));
}

/*****************************************************************/
//	Function:	writeushort
//	Purpose:	Writes an unsigned short (2 byte) to the message buffer
/*****************************************************************/
int writeushort(unsigned short val, int buffid)
{
	CBuffer* buff = buffers[buffid];
	return ((buff == NULL) ? 0 : buff->writeushort(val));
}

/*****************************************************************/
//	Function:	writeint
//	Purpose:	Writes an int (4 bytes) to the message buffer
/*****************************************************************/
int writeint(int val, int buffid)
{
	CBuffer* buff = buffers[buffid];
	return ((buff == NULL) ? 0 : buff->writeint(val));
}

/*****************************************************************/
//	Function:	writeuint
//	Purpose:	Writes an unigned int (4 bytes) to the message buffer
/*****************************************************************/
int writeuint(unsigned int val, int buffid)
{
	CBuffer* buff = buffers[buffid];
	return ((buff == NULL) ? 0 : buff->writeuint(val));
}

/*****************************************************************/
//	Function:	writefloat
//	Purpose:	Writes a float (4 bytes) to the message buffer
/*****************************************************************/
int writefloat(float val, int buffid)
{
	CBuffer* buff = buffers[buffid];
	return ((buff == NULL) ? 0 : buff->writefloat(val));
}

/*****************************************************************/
//	Function:	writedouble
//	Purpose:	Writes a double (8 bytes) to the message buffer
/*****************************************************************/
int writedouble(double val, int buffid)
{
	CBuffer* buff = buffers[buffid];
	return ((buff == NULL) ? 0 : buff->writedouble(val));
}

/*****************************************************************/
//	Function:	writestring
//	Purpose:	Writes a string (?? bytes) to the message buffer
/*****************************************************************/
int writestring(char* str, int buffid)
{
	CBuffer* buff = buffers[buffid];
	return ((buff == NULL) ? 0 : buff->writestring(str));
}

/*****************************************************************/
//	Function:	writestring
//	Purpose:	Writes a string (?? bytes) to the message buffer
/*****************************************************************/
int writestring(const char* str, int buffid)
{
	CBuffer* buff = buffers[buffid];
	return ((buff == NULL) ? 0 : buff->writestring(str));
}

/*****************************************************************/
//	Function:	readchar
//	Purpose:	Reads a char (1 byte) from the message buffer
/*****************************************************************/
unsigned char readchar( int buffid, bool peek)
{
	CBuffer* buff = buffers[buffid];
	return ((buff == NULL) ? 0 : buff->readchar( peek ));
}

/*****************************************************************/
//	Function:	readshort
//	Purpose:	Reads a short (2 bytes) from the message buffer
/*****************************************************************/
short readshort( int buffid, bool peek )
{
	CBuffer* buff = buffers[buffid];
	return ((buff == NULL) ? 0 : buff->readshort( peek ));
}

/*****************************************************************/
//	Function:	readushort
//	Purpose:	Reads an unsigned short (2 bytes) from the message buffer
/*****************************************************************/
unsigned short readushort( int buffid, bool peek )
{
	CBuffer* buff = buffers[buffid];
	return ((buff == NULL) ? 0 : buff->readushort( peek ));
}

/*****************************************************************/
//	Function:	readint
//	Purpose:	Reads an int (4 bytes) from the message buffer
/*****************************************************************/
int readint( int buffid, bool peek )
{
	CBuffer* buff = buffers[buffid];
	return ((buff == NULL) ? 0 : buff->readint( peek ));
}

/*****************************************************************/
//	Function:	readuint
//	Purpose:	Reads an unsigned int (4 bytes) from the message buffer
/*****************************************************************/
unsigned int readuint( int buffid, bool peek )
{
	CBuffer* buff = buffers[buffid];
	return ((buff == NULL) ? 0 : buff->readuint( peek ));
}

/*****************************************************************/
//	Function:	readfloat
//	Purpose:	Reads a float (4 bytes) from the message buffer
/*****************************************************************/
float readfloat( int buffid, bool peek )
{
	CBuffer* buff = buffers[buffid];
	return ((buff == NULL) ? 0 : buff->readfloat( peek ));
}

/*****************************************************************/
//	Function:	readdouble
//	Purpose:	Reads a double (8 bytes) from the message buffer
/*****************************************************************/
double readdouble( int buffid, bool peek )
{
	CBuffer* buff = buffers[buffid];
	return ((buff == NULL) ? 0 : buff->readdouble( peek ));
}

/*****************************************************************/
//	Function:	readstring
//	Purpose:	Reads a string (?? bytes) from the message buffer
/*****************************************************************/
char* readstring( int buffid, bool peek )
{
	CBuffer* buff = buffers[buffid];
	return ((buff == NULL) ? 0 : buff->readstring( peek ));
}

/*****************************************************************/
//	Function:	getpos
//	Purpose:	Returns the current position within the buffer
/*****************************************************************/
int getpos( bool readwrite, int buffid)
{
	CBuffer* buff = buffers[buffid];
	if (buff == NULL) return 0;
	return ( readwrite ? buff->readpos : buff->writepos );
}

/*****************************************************************/
//	Function:	clearbuffer
//	Purpose:	Clears all data from the buffer
/*****************************************************************/
int clearbuffer(int buffid)
{
	CBuffer* buff = buffers[buffid];
	if (buff == NULL) return 0;
	buff->clear();
	return 1;
}

/*****************************************************************/
//	Function:	buffsize
//	Purpose:	Returns the size of the given buffer
/*****************************************************************/
int buffsize(int buffid)
{
	CBuffer* buff = buffers[buffid];
	return ((buff == NULL) ? 0 : buff->count);
}

/*****************************************************************/
//	Function:	setpos
//	Purpose:	Jumps to the given position in the buffer
/*****************************************************************/
int setpos(int pos, int buffid)
{
	CBuffer* buff = buffers[buffid];
	if (buff == NULL) return 0;
	buff->readpos = pos;
	buff->writepos = pos;
	return pos;
}

/*****************************************************************/
//	Function:	bytesleft
//	Purpose:	Returns how many bytes are left to read in the message buffer
/*****************************************************************/
int bytesleft(int buffid)
{
	CBuffer* buff = buffers[buffid];
	return ((buff == NULL) ? 0 : buff->bytesleft());
}

/*****************************************************************/
//	Function:	createbuffer
//	Purpose:	Creates a new buffer
/*****************************************************************/
int createbuffer()
{
	CBuffer* buff = new CBuffer();
	return AddBuffer(buff);
}

/*****************************************************************/
//	Function:	freebuffer
//	Purpose:	Frees the given buffer
/*****************************************************************/
bool freebuffer(int buffid)
{
	if (buffid == 0) return false;
	CBuffer* buff = buffers[buffid];
	if (buff == NULL) return false;
	delete buff;
	buffers[buffid] = NULL;
	return true;
}

/*****************************************************************/
//	Function:	copybuffer
//	Purpose:	Copies a buffer from one location to another
/*****************************************************************/
bool copybuffer(int destinationid, int sourceid)
{
	CBuffer* destination = buffers[destinationid];
	if(destination == NULL) return false;
	CBuffer* source = buffers[sourceid];
	if(source == NULL) return false;
	destination->addBuffer(source);
	return true;

}

/*****************************************************************/
//	Function:	copybuffer2
//	Purpose:	Copies certain information from a buffer from one location to another
/*****************************************************************/
bool copybuffer2(int destinationid, int start, int len, int sourceid)
{
	CBuffer* destination = buffers[destinationid];
	if(destination == NULL) return false;
	CBuffer* source = buffers[sourceid];
	if(source == NULL) return false;
	destination->addBuffer(source->data+start, len);
	return true;

}

/*****************************************************************/
//	Function:	getmacaddress
//	Purpose:	Returns the current mac address
/*****************************************************************/
char* getmacaddress()
{
	return CTools::getmacaddress();
}

/*****************************************************************/
//	Function:	md5string
//	Purpose:	Returns an MD5 hash code given a string
/*****************************************************************/
char* md5string(char* str)
{
	return CTools::md5string(str);
}

/*****************************************************************/
//	Function:	md5buffer
//	Purpose:	Returns an MD5 hash string of the data inside of the buffer
/*****************************************************************/
char* md5buffer(int buffid)
{
	CBuffer* buff = buffers[buffid];
	return ((buff == NULL) ? 0 : CTools::md5buffer(buff));
}

/*****************************************************************/
//	Function:	bufferencrypt
//	Purpose:	Encrypts the current buffer with a password
/*****************************************************************/
bool bufferencrypt(char* pass, int buffid)
{
	CBuffer* buff = buffers[buffid];
	if (buff == NULL) return false;
	CTools::encryptedbuffer(buff, pass);
	return true;
}

/*****************************************************************/
//	Function:	adler32
//	Purpose:	Returns the ADLER Checksum of the data inside of the buffer
/*****************************************************************/
unsigned int adler32(int buffid)
{
	CBuffer* buff = buffers[buffid];
	return ((buff == NULL) ? 0 : CTools::adler32(buff));
}

/*****************************************************************/
//	Function:	bufferexists
//	Purpose:	Returns whether the given buffer exists
/*****************************************************************/
bool bufferexists(int buffid)
{
	CBuffer* buff = buffers[buffid];
	return (buff != 0);
}

/*****************************************************************/
//	Function:	fileopen
//	Purpose:	Opens a file for reading/writing
/*****************************************************************/
int fileopen(char* name, int mode)
{
	HANDLE file = CTools::BinOpen(name, mode);
	return ((file != 0) ? AddFile(file) : -1);
}

/*****************************************************************/
//	Function:	fileclose
//	Purpose:	Closes an open file
/*****************************************************************/
int fileclose(int fileid)
{
	HANDLE file = files[fileid];
	if (file == NULL) return -1;
	files[fileid] = NULL;
	return (int)CTools::BinClose(file);
}

/*****************************************************************/
//	Function:	filewrite
//	Purpose:	Writes a file from the information in the message buffer
/*****************************************************************/
int filewrite(int fileid, int buffid)
{
	HANDLE file = files[fileid];
	if (file == NULL) return -1;
	CBuffer* buff = buffers[buffid];
	if (buff == NULL) return -1;
	return CTools::BinWrite(file, buff);
}

/*****************************************************************/
//	Function:	fileread
//	Purpose:	Reads the given file into the message buffer
/*****************************************************************/
int fileread(int fileid, int bytes, int buffid)
{
	HANDLE file = files[fileid];
	if (file == NULL) return -1;
	CBuffer* buff = buffers[buffid];
	return ((buff == NULL) ? -1 : CTools::BinRead(file, bytes, buff));
}

/*****************************************************************/
//	Function:	filepos
//	Purpose:	Returns the current position within the file
/*****************************************************************/
int filepos(int fileid)
{
	HANDLE file = files[fileid];
	return ((file == NULL) ? -1 : CTools::BinGetPos(file));
}

/*****************************************************************/
//	Function:	filepos
//	Purpose:	Jumps to a given position within the file
/*****************************************************************/
int filesetpos(int fileid, int pos)
{
	HANDLE file = files[fileid];
	return ((file == NULL) ? -1 : CTools::BinSetPos(file, pos));
}

/*****************************************************************/
//	Function:	filesize
//	Purpose:	Returns the size of the given file
/*****************************************************************/
int filesize(int fileid)
{
	HANDLE file = files[fileid];
	return ((file == NULL) ? -1 : CTools::BinFileSize(file));
}

/*****************************************************************/
//	Function:	AddBuffer
//	Purpose:	Adds a buffer onto the buffer list
/*****************************************************************/
int AddBuffer(CBuffer* b)
{
	for(unsigned int i = 0; i < buffers.size(); i ++)
		if(buffers[i] == NULL)
		{
			buffers[i] = b;
			return i;
		}

	buffers.push_back(b);
	return int(buffers.size()) - 1;
}

/*****************************************************************/
//	Function:	AddSocket
//	Purpose:	Adds a socket onto the socket list
/*****************************************************************/
int AddSocket(CSocket* b)
{
	for(int i = 0; i < int(sockets.size()); i ++)
		if(sockets[i] == NULL)
		{
			sockets[i] = b;
			return i;
		}

	sockets.push_back(b);
	return int(sockets.size()) - 1;
}

/*****************************************************************/
//	Function:	AddFile
//	Purpose:	Adds a file onto the file list
/*****************************************************************/
int AddFile(HANDLE b)
{
	for(int i = 0; i < int(files.size()); i ++)
		if(files[i] == NULL)
		{
			files[i] = b;
			return i;
		}

	files.push_back(b);
	return int(files.size()) - 1;
}