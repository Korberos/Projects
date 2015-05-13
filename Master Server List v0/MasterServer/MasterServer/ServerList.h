#pragma once

#include <vector>
using namespace std;

#include "SingleLinkList.h"
#include "SOCK_functions.h"

#define SERVERS			0
#define CLIENTS			1
#define MAX_PING		3

#define SERVER_PORT		2345
#define CLIENT_PORT		2346

class ServerList
{
private:
	int ServerSocket[2];

	SLList<int>				S_Socket;
	SLList<char*>			S_IPAddr;
	SLList<char*>			S_Name;
	SLList<char>			S_PingCount;

	SLList<int>				C_Socket;
	SLList<char*>			C_IPAddr;
	SLList<char>			C_PingCount;

public:
	int						Server_Count;
	int						Client_Count;

	ServerList(void)	{}
	~ServerList(void)	{}

	bool Initialize(void);
	void MainProcess(void);
	void Shutdown(void);

	void AddServer(int SockID, char* IP, char* Name);
	void RemoveServer(int ID);
	void PingServers(double Step);
	void AcceptNewServers(void);
	void Messages_Servers(void);

	void AddClient(int SockID, char* IP);
	void RemoveClient(int ID);
	void PingClients(double Step);
	void AcceptNewClients(void);
	void Messages_Clients(void);
	void SendServerList(int CSocket);
	void SendChatMessage(char* String);

	// Accessors
	inline int GetServerSocket(int i)		{ return S_Socket[i];		}
	inline char* GetServerIP(int i)			{ return S_IPAddr[i];		}
	inline char* GetServerName(int i)		{ return S_Name[i];			}
	inline char* GetClientIP(int i)			{ return C_IPAddr[i];		}
};
