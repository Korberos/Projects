#include "Server.h"

bool Server::Initialize()
{
	// Initialize the Listening Port for Clients
	ServerSocket = tcplisten(SERVER_PORT, 10, 1);
	if (ServerSocket == -1) return false;
	setnagle(ServerSocket, true);

	// Connect to the MSL
	MSLSocket = tcpconnect(MSL_IP, MSL_PORT, 1);
	if (MSLSocket == -1) return false;

	Client_Count = 0;

	return true;
}

bool Server::MainProcess(void)
{
	// Get the current running time and step time
	static DWORD	TICK_Current		= GetTickCount();
	static DWORD	TICK_Last			= GetTickCount();
	static double	TIME_Step			= 0.0;
	TICK_Last = TICK_Current;
	TICK_Current = GetTickCount();
	TIME_Step = (((double)TICK_Current - (double)TICK_Last) / 1000.0);

	// Accept Incoming Connections
	AcceptNewClients();

	// Receive messages
	if (Messages_MSL() == 0) return false;
	Messages_Clients();

	return true;
}

void Server::Shutdown(void)
{
	closesock(ServerSocket);
	closesock(MSLSocket);
}

bool Server::Messages_MSL(void)
{
	int MessageBuffer = receivemessage(MSLSocket, 0, 0);
	if (MessageBuffer == 0) return false;
	if (MessageBuffer < 0) return true;

	char MessageID = readchar(0);

	switch (MessageID)
	{
	case 1:
		clearbuffer(0);
		writechar(1, 0);
		sendmessage(MSLSocket, MSL_IP, MSL_PORT, 0);
		break;
	}

	return true;
}

////////////////////////////////////////
//	Client Connection Functions
////////////////////////////////////////

void Server::AddClient(int SockID, char* IP)
{
	C_Socket.AddHead(SockID);
	C_IPAddr.AddHead(IP);

	Client_Count += 1;
}

void Server::RemoveClient(int ID)
{
	C_Socket.Remove(ID);
	C_IPAddr.Remove(ID);

	Client_Count -= 1;
}

void Server::AcceptNewClients(void)
{
	int NewClient = tcpaccept(ServerSocket, 1);
	while (NewClient >= 0)
	{
		AddClient(NewClient, tcpip(NewClient));

		// Check for another client connection
		NewClient = tcpaccept(ServerSocket, 1);
	}
}

void Server::Messages_Clients(void)
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
		switch(MessageID)
		{
		case 1:
			C_PingCount[i] = 0;
			break;

		case 2:
			SendChatString(readstring(0));
			break;

		case 3:
			char NewString[100];
			char* Name = readstring(0);
			char* Extra = " has entered the server.";
			int S = (int)strlen(Name);
			for (int i = 0; Name[i] != 0; i += 1)		NewString[i] = Name[i];
			for (int i = S; Extra[i-S] != 0; i += 1)	NewString[i] = Extra[i-S];
			S += (int)strlen(Extra);
			NewString[S] = 0;
			SendChatString(NewString);
		}
	}
}

////////////////////////////////////////
//	Program Functionality
////////////////////////////////////////

void Server::SendChatString(char* String)
{
	for (int i = 0; i < Client_Count; i += 1)
	{
		clearbuffer(0);
		writechar(2, 0);
		writestring(String, 0);
		sendmessage(C_Socket[i], C_IPAddr[i], SERVER_PORT, 0);
	}
}