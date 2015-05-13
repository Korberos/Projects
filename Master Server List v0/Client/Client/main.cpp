#include <iostream>
using namespace std;

#include <windows.h>

#include "SOCK_functions.h"
#include "Chat.h"

#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)

#define MSL_IP		"127.0.0.1"//"fullsailchat.hopto.org"
#define MSL_PORT	2346

char* FindServer(void);

void main(int argc, char** argv)
{
	//	Step 1: INITIALIZATION
	Winsock_Init();
	char*	IP;
	char	SERVER_IP[16] = "";
	IP = FindServer();
	for (int i = 0; IP[i] != 0; i += 1)
	{
		SERVER_IP[i] = IP[i];
	}
	if (strcmp("FAILURE", SERVER_IP) == 0)
	{
		cout << "Failed to connect to the Master Server List... shutting down." << endl;
		Winsock_Exit();
		return;
	}

	Chat CHAT;
	if (CHAT.Initialize(SERVER_IP))
	{
		//	Step 2: EXECUTION
		while (!KEY_DOWN(VK_F3))
			if (!CHAT.MainProcess()) break;
	}

	//	Step 3: SHUTDOWN
	CHAT.Shutdown();
	Winsock_Exit();
	system("cls");
}

char* FindServer(void)
{
	// Connect to MSL, receive the server list, and take an input to either refres the list, exit, or pick a server.
	int MSL_Socket = tcpconnect(MSL_IP, MSL_PORT, 1);
	if (MSL_Socket <= 0) return "FAILURE";
	clearbuffer(0);
	writechar(2,  0);
	sendmessage(MSL_Socket, MSL_IP, MSL_PORT, 0);

	while (true)
	{
		int Message = receivemessage(MSL_Socket, 0, 0);
		if (Message == 0) return "FAILURE";
		if (Message < 0) continue;

		system("cls");
		COORD C;
		C.X = 2;	C.Y = 2; SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), C);
		cout << "Input the selection identifier for the server you wish to join." << endl << "  =====================================================" << endl << "  ";

		char Server_IP[10][20];
		char* ReceivedIP = UINTtoIP(readuint(0));
		for (int i = 0; ReceivedIP[i-1] != 0; i += 1) Server_IP[0][i] = ReceivedIP[i];

		for (int i = 0; strcmp(Server_IP[i], "0.0.0.0"); i += 1)
		{
			cout << "#" << i + 1 << ":  " << readstring(0) << " ~ ";
			for (int j = 0; Server_IP[i][j] != 0; j += 1) cout << Server_IP[i][j];
			cout << endl << "  ";

			(i < 9) ? strcpy_s(Server_IP[i+1], 21, UINTtoIP(readuint(0))) : strcpy_s(Server_IP[i+1], 7, "0.0.0.0" );
		}

		int Selection;
		
		cout << endl << endl << "  Server Selection (0 to refresh): ";
		cin >> Selection;
		cin.get();
		if ((Selection < 1) || (Selection > 10))
		{
			clearbuffer(0);
			writechar(2,  0);
			sendmessage(MSL_Socket, MSL_IP, MSL_PORT, 0);
			continue;
		}
		else
		{
			closesocket(MSL_Socket);
			return Server_IP[Selection-1];
		}
	}

	return "FAILURE";
}