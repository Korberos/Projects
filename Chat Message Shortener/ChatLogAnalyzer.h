#include <string>
#include <fstream>
#include <vector>
#include <queue>

class ChatLogAnalyzer
{
public:
	ChatLogAnalyzer() : MaxComboLength(0), UnusedCharacters(256), SizeOfLogFile(0), NewSizeOfLogFile(0) {}
	bool Analyze(char* chat_log_file, int max_combo_length);

	class CharacterCombo
	{
	public:
		CharacterCombo() : ComboString(""), Score(0) {}

		std::string ComboString;
		int Score;
		std::vector<int> LocationList;

		void Scoring(void)	{	Score = int(LocationList.size()) * (int(ComboString.size()) - 1);	}
		void operator=(const CharacterCombo& other)
		{
			ComboString = other.ComboString;
			Score = other.Score;
			LocationList.clear();
			for (int i = 0; i < int(other.LocationList.size()); ++i) LocationList.push_back(other.LocationList[i]);
		}
	};

private:
	int MaxComboLength;
	int UnusedCharacters;
	int FirstUnusedCharacter;
	int SizeOfLogFile;
	int NewSizeOfLogFile;
	std::vector<std::string> StringCollection;
	std::vector<CharacterCombo> ComboList;
	std::pair<bool, std::string> AsciiTable[256];
	std::string TempCombo;
	std::vector<CharacterCombo> HighScoreCombos;

	bool LoadStringCollection(char* chat_log_file);
	void CreateAsciiTable(void);
	bool FindFirstUnusedCharacter(void);
	void FindBestCombinations(void);
	void CalculateByteSavings(void);
	bool OutputNewAsciiList(void);
};

bool operator<(const ChatLogAnalyzer::CharacterCombo left, const ChatLogAnalyzer::CharacterCombo right) { return left.Score < right.Score;	}
bool operator>(const ChatLogAnalyzer::CharacterCombo left, const ChatLogAnalyzer::CharacterCombo right) { return left.Score > right.Score;	}

bool ChatLogAnalyzer::Analyze(char* chat_log_file, int max_combo_length)
{
	if (max_combo_length < 2)					return false;
	MaxComboLength = max_combo_length;

	printf("Reading In Chat Log File... ");
	if (!LoadStringCollection(chat_log_file))	return false;
	printf("COMPLETE  \n");

	printf("Creating ASCII Existance Table...  ");
	CreateAsciiTable();
	printf("COMPLETE (%d available)  \n", UnusedCharacters);

	printf("Finding the best combinations... \n\n");
	FindBestCombinations();
	printf("COMPLETE  \n");

	CalculateByteSavings();
	if (!OutputNewAsciiList()) return false;

	return true;
}

////////////////////////////////////////
//  LoadStringCollection
//  
//  - Opens the chat log file and reads
//    in all chat strings into a list
////////////////////////////////////////
bool ChatLogAnalyzer::LoadStringCollection(char* chat_log_file)
{
	if (!chat_log_file)			return false;

	//  Open the chat log
	std::ifstream ChatLogStream(chat_log_file);
	if (ChatLogStream.bad())	return false;
	if (!ChatLogStream.good())	return false;

	//  Read in each line from the chat log (this should at least be a couple hundred, if not a few thousand)
	SizeOfLogFile = 0;
	char TempString[2048];
	while (!ChatLogStream.eof())
	{
		ChatLogStream.getline(TempString, 2048);
		StringCollection.push_back(std::string(TempString));
		SizeOfLogFile += int(strlen(TempString));
	}

	//  Close the chat log
	ChatLogStream.close();
	return true;
}

////////////////////////////////////////
//  CreateAsciiTable
//  
//  - Creates a table that outlines the
//    ascii values that are not in use
////////////////////////////////////////
void ChatLogAnalyzer::CreateAsciiTable(void)
{
	// Create a table of all characters that exist within the ASCII character set
	for (int i = 0; i < 256; ++i)
	{
		AsciiTable[i].first = false;
	}

	// Loop through every chat line and identify existing characters in the AsciiTable
	for (int i = 0; i < int(StringCollection.size()); ++i)
	{
		for (int j = 0; j < int(StringCollection[i].size()); ++j)
		{
			if (AsciiTable[StringCollection[i][j]].first == false)
			{
				--UnusedCharacters;
				AsciiTable[StringCollection[i][j]].second += StringCollection[i][j];
			}
			AsciiTable[StringCollection[i][j]].first = true;
		}
	}
}

////////////////////////////////////////
//  FindFirstUnusedCharacter
//  
//  - Finds the first unused character
////////////////////////////////////////
bool ChatLogAnalyzer::FindFirstUnusedCharacter(void)
{
	for (int i = 0; i < 256; ++i)
	{
		if (AsciiTable[i].first == false)
		{
			FirstUnusedCharacter = i;
			AsciiTable[i].first = true;
			return true;
		}
	}

	return false;
}

////////////////////////////////////////
//  FindBestCombinations
//  
//  - Once for each of the unused ascii
//    characters, the program finds the
//    best combination to shorten
////////////////////////////////////////
void ChatLogAnalyzer::FindBestCombinations()
{
	if (FindFirstUnusedCharacter() == false) return;

	//  Clear the combination list
	ComboList.clear();

	//  Loop through every chat line and increment all combinations smaller than MAX_COMBO_LENGTH
	for (int i = 0; i < int(StringCollection.size()); ++i)
	{
		for (int j = MaxComboLength; j > 1; --j)
		{
			for (int k = 0; k < int(StringCollection[i].size()) - j; ++k)
			{
				TempCombo = "";
				for (int l = 0; l < j; ++l) TempCombo += StringCollection[i][k + l];

				bool StringExistsInComboList = false;
				for (int m = 0; m < int(ComboList.size()); ++m)
				{
					if (ComboList[m].ComboString == TempCombo)
					{
						ComboList[m].LocationList.push_back(i);
						StringExistsInComboList = true;
						break;
					}
				}
				if (StringExistsInComboList == false)
				{
					CharacterCombo NewCombo;
					NewCombo.ComboString = TempCombo;
					NewCombo.LocationList.push_back(i);
					ComboList.push_back(NewCombo);
				}
			}
		}
	}

	//  Now that you have all possible combinations, give each a score based on the bytes they would save if optimized, then find the best
	CharacterCombo HighestScore = ComboList[0];
	for (int i = 0; i < int(ComboList.size()); ++i)
	{
		ComboList[i].Scoring();
		if (ComboList[i] > HighestScore) HighestScore = ComboList[i];
	}

	//  Push the highest scorer into a list for later
	HighScoreCombos.push_back(HighestScore);

	//  Set the character in the Ascii Table to contain the combo string
	AsciiTable[FirstUnusedCharacter].second = HighestScore.ComboString;

	//  Go through the log file string collection and replace all instances of the HighestScore combination with the first unused character
	for (int i = 0; i < int(HighestScore.LocationList.size()); ++i)
	{
		std::string NewString;
		int index = int(StringCollection[HighestScore.LocationList[i]].find(HighestScore.ComboString));
		for (int j = 0; j < index; ++j) NewString += StringCollection[HighestScore.LocationList[i]][j];
		NewString += FirstUnusedCharacter;
		for (int j = index + int(HighestScore.ComboString.size()); j < int(StringCollection[HighestScore.LocationList[i]].size()); ++j) NewString += StringCollection[HighestScore.LocationList[i]][j];
		StringCollection[HighestScore.LocationList[i]] = NewString;
	}

	//  Output the selected character combo
	printf("[%d bytes saved]\t(%d)\t", HighestScore.Score, --UnusedCharacters);
	for (int i = 0; i < int(HighestScore.ComboString.size()); ++i) printf("%d  ", int(unsigned char(HighestScore.ComboString.c_str()[i])));
	printf("\n");

	// Recursively call the function until there are no more unused characters
	FindBestCombinations();
}

////////////////////////////////////////
//  CalculateByteSavings
//  
//  - Calculate and output the amount
//    of bytes you would save with the
//    given chat log, if you used the
//    string shortener table
////////////////////////////////////////
void ChatLogAnalyzer::CalculateByteSavings(void)
{
	NewSizeOfLogFile = SizeOfLogFile;
	for (int i = 0; i < int(HighScoreCombos.size()); ++i) NewSizeOfLogFile -= HighScoreCombos[i].Score;

	printf("\n");
	printf("Old log file size:    %d  \n", SizeOfLogFile);
	printf("New log file size:    %d  \n", NewSizeOfLogFile);
}

////////////////////////////////////////
//  OutputNewAsciiList
//  
//  - Outputs a file that contains
//    a list of all characters and
//    their corresponding string
//    
////////////////////////////////////////
bool ChatLogAnalyzer::OutputNewAsciiList(void)
{
	char WriteString[1024];

	std::ofstream OutputStream("NewAsciiTable.xml");
	if (OutputStream.bad()) return false;
	if (!OutputStream.good()) return false;

	OutputStream.write((char*)"<AsciiConversionTable>\n", int(strlen("<AsciiConversionTable>\n")) + 1);
	
	for (int i = 0; i < 256; ++i)
	{
		sprintf(WriteString, "\t<Ascii_%d>", i);
		OutputStream.write((char*)WriteString, int(strlen(WriteString)));

		if (AsciiTable[i].second.size())
		{
			sprintf(WriteString, "\n", i);
			OutputStream.write((char*)WriteString, int(strlen(WriteString)));
		}

		for (int j = 0; j < int(AsciiTable[i].second.size()); ++j)
		{
			sprintf(WriteString, "\t\t<Value>%d</Value>\n", int(unsigned char(AsciiTable[i].second[j])));
			OutputStream.write((char*)WriteString, int(strlen(WriteString)));
		} 

		if (AsciiTable[i].second.size())
		{
			sprintf(WriteString, "\t", i);
			OutputStream.write((char*)WriteString, int(strlen(WriteString)));
		}

		sprintf(WriteString, "</Ascii_%d>\n", i);
		OutputStream.write((char*)WriteString, int(strlen(WriteString)));
	}

	OutputStream.write((char*)"</AsciiConversionTable>", int(strlen("</AsciiConversionTable>")));

	OutputStream.close();
	return true;
}