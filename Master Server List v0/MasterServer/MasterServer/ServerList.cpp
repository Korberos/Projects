#include "ServerList.h"

#include "SOCK_functions.h"

bool ServerList::Initialize(void)
{
	// Initialize the Listening Port for Servers
	ServerSocket[SERVERS] = tcplisten(SERVER_PORT, 0, 1);
	if (ServerSocket[SERVERS] == -1) return false;
	setnagle(ServerSocket[SERVERS], true);

	// Initialize the Listening Port for Clients
	ServerSocket[CLIENTS] = tcplisten(CLIENT_PORT, 0, 1);
	if (ServerSocket[CLIENTS] == -1) return false;
	setnagle(ServerSocket[CLIENTS], true);

	Server_Count = 0;
	Client_Count = 0;

	return true;
}

void ServerList::MainProcess(void)
{
	// Get the current running time and step time
	static DWORD	TICK_Current		= GetTickCount();
	static DWORD	TICK_Last			= GetTickCount();
	static double	TIME_Step			= 0.0;
	TICK_Last = TICK_Current;
	TICK_Current = GetTickCount();
	TIME_Step = (((double)TICK_Current - (double)TICK_Last) / 1000.0);

	// Ping all connections
	PingServers(TIME_Step);
	PingClients(TIME_Step);

	// Accept Incoming Connections
	AcceptNewServers();
	AcceptNewClients();

	// Receive messages
	Messages_Servers();
	Messages_Clients();
}

void ServerList::Shutdown(void)
{
	closesock(ServerSocket[SERVERS]);
	closesock(ServerSocket[CLIENTS]);
}


////////////////////////////////////////
//	Server Connection Functions
////////////////////////////////////////

void ServerList::AddServer(int SockID, char* IP, char* Name)
{
	S_Socket.AddHead(SockID);
	S_IPAddr.AddHead(IP);
	S_Name.AddHead(Name);
	S_PingCount.AddHead(0);

	Server_Count += 1;
}

void ServerList::RemoveServer(int ID)
{
	S_Socket.Remove(ID);
	S_IPAddr.Remove(ID);
	S_Name.Remove(ID);
	S_PingCount.Remove(ID);

	Server_Count -= 1;
}



void ServerList::PingServers(double Step)
{
	// Only run this every 5 seconds
	static double TIME_PingOffset = 0.0;
	TIME_PingOffset += Step;
	if (TIME_PingOffset < 5.0) return;
	TIME_PingOffset -= 5.0;

	// Send the ping character (1) to all servers
	for (int i = 0; i < S_Socket.Size(); i += 1)
	{
		S_PingCount[i] += 1;
		if (S_PingCount[i] > MAX_PING)
		{
			RemoveServer(i);
			i -= 1;
			continue;
		}

		clearbuffer(0);
		writechar(1, 0);
		sendmessage(S_Socket[i], S_IPAddr[i], SERVER_PORT, 0);
	}
}

void ServerList::AcceptNewServers(void)
{
	int NewServer = tcpaccept(ServerSocket[SERVERS], 1);
	while (NewServer >= 0)
	{
		AddServer(NewServer, tcpip(NewServer), "::NAME UNKNOWN::");

		// Check for another server connection
		NewServer = tcpaccept(ServerSocket[SERVERS], 1);
	}
}

void ServerList::Messages_Servers(void)
{
	for (int i = 0; i < S_Socket.Size(); i += 1)
	{
		int MessageBuffer = receivemessage(S_Socket[i], 0, 0);
		if (MessageBuffer == 0)
		{
			RemoveServer(i);
			i -= 1;
			continue;
		}
		if (MessageBuffer < 0) continue;

		char MessageID = readchar(0);

		switch (MessageID)
		{
		case 1:
			S_PingCount[i] = 0;
			break;
		case 2:
			S_Name[i] = readstring(0);
			S_PingCount[i] = 0;
			break;
		}
	}
}

////////////////////////////////////////
//	Client Connection Functions
////////////////////////////////////////

void ServerList::AddClient(int SockID, char* IP)
{
	C_Socket.AddHead(SockID);
	C_IPAddr.AddHead(IP);
	C_PingCount.AddHead(0);

	Client_Count += 1;
}

void ServerList::RemoveClient(int ID)
{
	C_Socket.Remove(ID);
	C_IPAddr.Remove(ID);
	C_PingCount.Remove(ID);

	Client_Count -= 1;
}

void ServerList::PingClients(double Step)
{
	// Only run this every 5 seconds
	static double TIME_PingOffset = 0.0;
	TIME_PingOffset += Step;
	if (TIME_PingOffset < 5.0) return;
	TIME_PingOffset -= 5.0;

	// Send the ping character (1) to all servers
	for (int i = 0; i < C_Socket.Size(); i += 1)
	{
		C_PingCount[i] += 1;
		if (C_PingCount[i] > MAX_PING)
		{
			RemoveClient(i);
			i -= 1;
			continue;
		}

		clearbuffer(0);
		writechar(1, 0);
		sendmessage(C_Socket[i], C_IPAddr[i], CLIENT_PORT, 0);
	}
}

void ServerList::AcceptNewClients(void)
{
	int NewClient = tcpaccept(ServerSocket[CLIENTS], 1);
	while (NewClient >= 0)
	{
		AddClient(NewClient, tcpip(NewClient));

		// Check for another client connection
		NewClient = tcpaccept(ServerSocket[CLIENTS], 1);
	}
}

void ServerList::Messages_Clients(void)
{
	for (int i = 0; i < C_Socket.Size(); i += 1)
	{
		int MessageBuffer = receivemessage(C_Socket[i], 0, 0);
		if (MessageBuffer == 0)
		{
			RemoveClient(i);
			i -= 1;
			continue;
		}
		if (MessageBuffer < 0) continue;

		char MessageID = readchar(0);

		switch (MessageID)
		{
		case 1:
			S_PingCount[i] = 0;
			break;
		case 2:
			SendServerList(C_Socket[i]);
			break;
		}
	}
}

void ServerList::SendServerList(int CSocket)
{
	clearbuffer(0);
	for (int i = 0; i < S_Socket.Size(); i += 1)
	{
		writeuint(IPtoUINT(S_IPAddr[i]), 0);
		char* test = S_Name[i];
		writestring(test, 0);
	}
	writeuint(IPtoUINT("0.0.0.0"), 0);
	sendmessage(CSocket, tcpip(CSocket), CLIENT_PORT, 0);
}