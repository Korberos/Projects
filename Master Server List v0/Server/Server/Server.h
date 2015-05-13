#pragma once

#include "SingleLinkList.h"
#include "SOCK_functions.h"

#define MSL_IP			"127.0.0.1"//"fullsailchat.hopto.org"
#define MSL_PORT		2345

#define SERVER_PORT		2347

class Server
{
private:
	int						ServerSocket;
	int						MSLSocket;

	SLList<int>				C_Socket;
	SLList<char*>			C_IPAddr;
	SLList<char>			C_PingCount;

public:
	int						Client_Count;

	Server(void)	{}
	~Server(void)	{}

	bool Initialize(void);
	bool MainProcess(void);
	void Shutdown(void);
	
	bool Messages_MSL(void);

	void AddClient(int SockID, char* IP);
	void RemoveClient(int ID);
	void AcceptNewClients(void);
	void Messages_Clients(void);

	void SendChatString(char* String);

	inline char*	GetClientIP(int i)		{ return C_IPAddr[i]; 	}
	inline int		GetMSLSocket(void)		
	{
		return MSLSocket;			
	}
};
