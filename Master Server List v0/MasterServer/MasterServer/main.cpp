#include <iostream>
using namespace std;

#include <windows.h>

#include "SOCK_functions.h"
#include "ServerList.h"

#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)

void Display_INIT(void);
void Display_DEBUG(ServerList* MSL);

void main(int argc, char** argv)
{
	bool DEBUG_MODE = true;
	if (argc > 1) if (!strcmp(argv[1], "debug")) DEBUG_MODE = true;

	//	Step 1: INITIALIZATION
	Winsock_Init();
	ServerList MSL;
	if (MSL.Initialize())
	{
		Display_INIT();

		//	Step 2: EXECUTION
		while (!KEY_DOWN(VK_F1))
		{
			MSL.MainProcess();

			// Debug data
			if (DEBUG_MODE) Display_DEBUG(&MSL);
		}
	}

	//	Step 3: SHUTDOWN
	system("cls");
	MSL.Shutdown();
	Winsock_Exit();
}

void Display_INIT(void)
{
	COORD C;

	system("cls");
	cout << "~Server Initialized!" << endl;
	cout << "~Beginning Server Duties..." << endl;
	cout << "================================================================================";
	C.X = 0; C.Y = 24; SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), C);
	cout << "================================================================================";

	C.X = 0;	C.Y = 0;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), C);
}

void Display_DEBUG(ServerList* MSL)
{
	COORD C;

	C.X = 2;	C.Y = 4;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), C);
	cout << "Connected Servers: " << MSL->Server_Count << "   " << endl;

	C.X = 2;	C.Y = 5;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), C);
	cout << "Connected Clients: " << MSL->Client_Count << "   " << endl;

	C.X = 2;	C.Y = 7;	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), C);
	cout << "DEBUG DISPLAY: FIRST TEN SERVERS";
	C.X = 2;	C.Y = 8;	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), C);
	cout << "================================";

	int DebugCount = 10;
	for (int i = 0; i < DebugCount; i += 1)
	{
		if (i < MSL->Server_Count)
		{
			C.X = 2;	C.Y = 9 + i;
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), C);
			cout << MSL->GetServerSocket(i) << "                    ";

			C.X = 2;	C.Y = 9 + i;
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), C);
			cout << MSL->GetServerName(i) << "                    ";

			C.X = 32;	C.Y = 9 + i;
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), C);
			cout << MSL->GetServerIP(i)  << "                    " << endl;
		}
		else
		{
			C.X = 2;	C.Y = 9 + i;
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), C);
			cout << "                                                            ";
		}
	}
}