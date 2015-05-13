#include <iostream>
using namespace std;

#include <windows.h>

#include "SOCK_functions.h"
#include "Server.h"

#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)

void Server_INIT(void);
void Display_INIT(void);
void Display_DEBUG(Server* SERVER);

char	Server_Name[100];

int		TestInt;

void main(int argc, char** argv)
{
	bool DEBUG_MODE = true;
	if (argc > 1) if (!strcmp(argv[1], "debug")) DEBUG_MODE = true;

	//	Step 1: INITIALIZATION
	Server_INIT();
	Winsock_Init();
	Server SERVER;
	if (SERVER.Initialize())
	{
		// Send information to the MSL
		clearbuffer(0);
		writechar(2, 0);
		writestring(Server_Name, 0);
		sendmessage(SERVER.GetMSLSocket(), MSL_IP, MSL_PORT, 0);

		Display_INIT();

		//	Step 2: EXECUTION
		while (!KEY_DOWN(VK_F2))
		{
			if (SERVER.MainProcess() == 0)
			{
				system("cls");
				cout << "Lost connection to the MSL" << endl;
				break;
			}

			// Debug data
			if (DEBUG_MODE) Display_DEBUG(&SERVER);
		}
	}
	else
	{
		system("cls");
		cout << "Failed to initialize the server." << endl;
	}

	//	Step 3: SHUTDOWN
	SERVER.Shutdown();
	Winsock_Exit();
	system("cls");
}

void Server_INIT(void)
{
	COORD C;

	system("cls");
	C.X = 0; C.Y = 0; SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), C);
	cout << "Input Server Name (Max 100 characters): ";
	cin.get(Server_Name, 100);
	cin.get();
}

void Display_INIT(void)
{
	COORD C;

	system("cls");
	C.X = 0; C.Y = 0; SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), C);
	cout << "~Server Initialized!" << endl;
	cout << "~Beginning Server Duties..." << endl;
	cout << "================================================================================";
	C.X = 0; C.Y = 24; SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), C);
	cout << "================================================================================";

	C.X = 0;	C.Y = 0;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), C);
}

void Display_DEBUG(Server* SERVER)
{
	COORD C;

	C.X = 3;	C.Y = 5;	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), C);
	cout << "DEBUG DISPLAY: FIRST TEN CLIENTS";
	C.X = 3;	C.Y = 6;	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), C);
	cout << "================================";

	int DebugCount = 10;
	for (int i = 0; i < DebugCount; i += 1)
	{
		if (i < SERVER->Client_Count)
		{
			C.X = 3;	C.Y = 7 + i;	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), C);
			cout << "#" << i+1 << ":  " << SERVER->GetClientIP(i) << "                    ";
		}
		else
		{
			C.X = 3;	C.Y = 7 + i;	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), C);
			cout << "                                        ";
		}
	}
}