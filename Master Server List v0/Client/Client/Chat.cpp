#include "Chat.h"

#include <iostream>
using namespace std;

#include "SOCK_functions.h"

#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)

bool Chat::Initialize(char* IP)
{
	for (int i = 0; IP[i-1] != 0; i += 1) ServerIP[i] = IP[i];

	// Connect to the given server
	ServerSocket = tcpconnect(ServerIP, SERVER_PORT, 1);
	if (ServerSocket <= 0) return false;

	system("cls");
	cout << " Please enter a name for yourself (<16 characters): ";
	cin.get(Name, 16);
	cin.get();

	if (strcmp(Name, "") == 0) strcpy_s(Name, strlen("Guest") + 1, "Guest");
	system("cls");

	// Send name to the server
	clearbuffer(0);
	writechar(3, 0);
	writestring(Name, 0);
	sendmessage(ServerSocket, ServerIP, SERVER_PORT, 0);

	// Clear the dialogue
	for (int i = 0; i < DIALOGUE_LINES; i += 1) Dialogue[i][0] = 0;
	for (int i = 0; i < MAX_LINE_LENGTH; i += 1) InputLine[i] = 0;
	NoDrawNeeded[0] = false;

	for (int i = 0; i < VALID_KEYS; i += 1) NoRepeat[i] = false;

	DrawOutline();
	
	return true;
}

bool Chat::MainProcess(void)
{
	ReadMessages();
	DisplayLines();
	CheckInput();
	return true;
}

void Chat::Shutdown(void)
{
	closesocket(ServerSocket);
}

bool Chat::ReadMessages(void)
{
	int MessageBuffer = receivemessage(ServerSocket, 0, 0);
	if (MessageBuffer == 0) return false;
	if (MessageBuffer < 0) return true;

	char MessageID = readchar(0);

	switch (MessageID)
	{
	case 1:
		clearbuffer(0);
		writechar(1, 0);
		sendmessage(ServerSocket, ServerIP, SERVER_PORT, 0);
		break;
	case 2:
		NewLine(readstring(0));
		break;
	}

	return true;
}

void Chat::NewLine(char* NewString)
{
	int Lines = (int)strlen(NewString) / MAX_LINE_LENGTH + 1;

	// Move all existing lines upward the number of line being added
	for (int i = Lines; i < DIALOGUE_LINES; i += 1)
	{
		for (int j = 0; j < MAX_LINE_LENGTH; j += 1) Dialogue[i-Lines][j] = Dialogue[i][j];
		Dialogue[i-Lines][MAX_LINE_LENGTH] = 0;
	}

	// Add the new lines
	for (int i = 0; i < Lines; i += 1)
	{
		for (int j = 0; j < MAX_LINE_LENGTH; j += 1) Dialogue[DIALOGUE_LINES - 1 - i][j] = 0;
		for (int j = 0; j < MAX_LINE_LENGTH; j += 1) Dialogue[DIALOGUE_LINES - 1 - i][j] = NewString[(Lines - 1 - i) * MAX_LINE_LENGTH + j];
		Dialogue[DIALOGUE_LINES - 1 - i][MAX_LINE_LENGTH] = 0;
	}

	NoDrawNeeded[0] = false;
}

void Chat::DisplayLines(void)
{
	if (!NoDrawNeeded[0])
	{
		system("cls");
		DrawOutline();

		COORD C;
		for (int i = 0; i < DIALOGUE_LINES; i += 1)
		{
			C.X = 2;	C.Y = 1 + i;	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), C);
			for (int j = 0; Dialogue[i][j] != 0; j += 1)
			{
				cout << Dialogue[i][j];
			}
		}

		NoDrawNeeded[0] = true;
	}
	if (!NoDrawNeeded[1])
	{
		COORD C;
		int i;

		// Draw the client's input string
		C.X = 2;	C.Y = 23;	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), C);
		for (i = 0; i < MAX_LINE_LENGTH; i += 1) cout << " ";

		C.X = 2;	C.Y = 23;	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), C);
		for (i = 0; InputLine[i] != 0; i += 1) cout << InputLine[i];

		
		C.X = 2 + i;	C.Y = 23;	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), C);

		NoDrawNeeded[1] = true;
	}
}

void Chat::CheckInput(void)
{
	// If the client detects the ENTER/RETURN key, send the current line
	if (KEY_DOWN(VK_RETURN))
	{
		if (NoRepeat[26] == false)
		{
			SendChatString(InputLine);
			InputLine[0] = 0;
			NoRepeat[26] = true;
		}
	}
	else NoRepeat[26] = false;

	// If the client detects the BACKSPACE key, go back one character
	if (KEY_DOWN(VK_BACK))
	{
		if (NoRepeat[27] == false)
		{
			int i = 0;
			for (i = 0; InputLine[i] != 0; i += 1) {}
			if (i != 0) InputLine[i-1] = 0;
			NoRepeat[27] = true;
			NoDrawNeeded[1] = false;
		}
	}
	else NoRepeat[27] = false;

	int PressedKey = 0;

	// If the client detects a letter, place it into the current input string
	for (int i = 'A'; i < 'A' + 26; i += 1)
	{
		if (KEY_DOWN(i)) 
		{
			if (NoRepeat[i - 'A'] == false)
			{
				PressedKey = i;
				NoRepeat[i - 'A'] = true;
			}
		}
		else NoRepeat[i - 'A'] = false;
	}

	// If the client detects the spacebar, add a space to the current string (overwrites any letter input)
	if (KEY_DOWN(VK_SPACE))
	{
		if (NoRepeat[28] == false)
		PressedKey = ' ';
		NoRepeat[28] = true;
	}
	else NoRepeat[28] = false;

	if (PressedKey)
	{
		unsigned int j;
		for (j = 0; ((InputLine[j] != 0) && (j < MAX_LINE_LENGTH - strlen(Name) - 2)); j += 1) {}
		if (j < (MAX_LINE_LENGTH - strlen(Name) - 2))
		{
			if (((PressedKey >= 'A') && (PressedKey <= 'Z')) || ((PressedKey >= 'a') && (PressedKey <= 'z')))
				PressedKey += ((KEY_DOWN(VK_SHIFT)) ? 0 : 32);

			InputLine[j] = PressedKey;
			InputLine[j+1] = 0;
			NoRepeat[PressedKey - 'A'] = true;
			NoDrawNeeded[1] = false;
		}
	}
}

void Chat::DrawOutline(void)
{
	COORD C;
	C.X = 0;	C.Y = 0;	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), C);
	for (int i = 0; i < 80; i += 1) cout << "-";
	for (int i = 1; i < DIALOGUE_LINES+1; i += 1)
	{
		C.X = 0;	C.Y = i;	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), C);
		cout << "|";
		C.X = 79;	C.Y = i;	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), C);
		cout << "|";
	}
	for (int i = 0; i < 80; i += 1) cout << "-";
	C.X = 0;	C.Y = 0;	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), C);
}

void Chat::SendChatString(char* String)
{
	char NewString[100];

	int S = (int)strlen(Name) + 2;
	for (int i = 0; Name[i] != 0; i += 1)		NewString[i] = Name[i];
	NewString[S-2] = ':';
	NewString[S-1] = ' ';
	for (int i = S; String[i-S] != 0; i += 1)	NewString[i] = String[i-S];
	S += (int)strlen(String);
	NewString[S] = 0;

	clearbuffer(0);
	writechar(2, 0);
	writestring(NewString, 0);
	sendmessage(ServerSocket, ServerIP, SERVER_PORT, 0);
}