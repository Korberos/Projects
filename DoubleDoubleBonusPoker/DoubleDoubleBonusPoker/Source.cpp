#include <iostream>
using namespace std;

#include <fstream>
using namespace std;

#include "Poker.h"

void CreateDataFile()
{
	int index = 0;
	for (unsigned char i1 = 0; i1 < 48; ++i1)
        for (unsigned char i2 = i1 + 1; i2 < 49; ++i2)
            for (unsigned char i3 = i2 + 1; i3 < 50; ++i3)
                for (unsigned char i4 = i3 + 1; i4 < 51; ++i4)
                    for (unsigned char i5 = i4 + 1; i5 < 52; ++i5)
                        HandValueData[index++] = CheckHand(i1, i2, i3, i4, i5);

	ofstream handValuesFile("HandValues.dat", ios_base::binary | ios_base::trunc);
	handValuesFile.write((const char*)(HandValueData), sizeof(double) * MAX_RIVER_LAYOUTS);
	handValuesFile.close();
    OutputStatistics();
}

void LoadDataFile()
{
	ifstream handValuesFile("HandValues.dat", ios_base::binary);
	handValuesFile.read((char*)(HandValueData), sizeof(double) * MAX_RIVER_LAYOUTS);
	handValuesFile.close();
}

void LoadBestPlayData()
{
	ifstream counterFile("CurrentCounter.txt");
	unsigned long int currentCounter = 0;
	counterFile >> currentCounter;
	counterFile.close();

	ifstream inputFile("BestPlay.dat", ios::binary);
	inputFile.read((char*)(&OutputData), sizeof(OutputData));
	inputFile.close();
}

void WriteBestPlayData(unsigned long int iterator, unsigned long int currentCounter)
{
	// Set the new river and copy the entire data to the data file
	ofstream flagOutput("BestPlay.dat", ios_base::trunc | ios_base::binary);
	flagOutput.write((const char*)(&OutputData), sizeof(OutputDataFormat));
	flagOutput.close();

	ofstream counterFile("CurrentCounter.txt");
	counterFile.clear();
	counterFile << currentCounter;
	counterFile.close();
}

void DetermineBestMoves()
{
	ifstream counterFile("CurrentCounter.txt");
	unsigned long int currentCounter = 0;
	counterFile >> currentCounter;
	counterFile.close();
	
	unsigned long int iterator = 0;
	for (unsigned char i1 = 0; i1 < 48; ++i1)
		for (unsigned char i2 = i1 + 1; i2 < 49; ++i2)
			for (unsigned char i3 = i2 + 1; i3 < 50; ++i3)
				for (unsigned char i4 = i3 + 1; i4 < 51; ++i4)
					for (unsigned char i5 = i4 + 1; i5 < 52; ++i5)
					{
						if (iterator++ < currentCounter) continue;

						River newRiver(i1, i2, i3, i4, i5);
						SetFinishedBitFlag(iterator - 1);
						DetermineBestMove(newRiver);
						OutputData.RiverList[iterator - 1] = newRiver;

						WriteBestPlayData(iterator, currentCounter);
						cout << "The best move for hand #" << ++currentCounter << " - " << DescribeHand(i1, i2, i3, i4, i5) << " is " << DescribeMove(newRiver.BestMove) << std::endl;
					}

    OutputStatistics();
}

void main()
{
    InitializeStatistics();
	LoadBestPlayData();
	LoadDataFile();
	DetermineBestMoves();
}