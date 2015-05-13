#pragma once

#define SERVER_PORT	2347

#define DIALOGUE_LINES		21
#define MAX_LINE_LENGTH		75
#define VALID_KEYS			29

class Chat
{
private:
	int			ServerSocket;
	char		ServerIP[16];

	char		Name[16];
	char		Dialogue[DIALOGUE_LINES][MAX_LINE_LENGTH + 1];
	bool		NoDrawNeeded[2];
	char		InputLine[MAX_LINE_LENGTH];

	char		NoRepeat[VALID_KEYS];

public:
	Chat(void)	{}
	~Chat(void)	{}

	bool Initialize(char* IP);
	bool MainProcess(void);
	void Shutdown(void);

	bool ReadMessages(void);
	void NewLine(char* NewString);
	void DisplayLines(void);
	void CheckInput(void);
	void SendChatString(char* String);

	void DrawOutline(void);
};