#include <iostream>
using namespace std;

#include <fstream>
using namespace std;

#include "Poker.h"

void CreateDataFile()
{
	int index = 0;
	for (int i1 = 0; i1 < 48; ++i1)
        for (int i2 = i1 + 1; i2 < 49; ++i2)
            for (int i3 = i2 + 1; i3 < 50; ++i3)
                for (int i4 = i3 + 1; i4 < 51; ++i4)
                    for (int i5 = i4 + 1; i5 < 52; ++i5)
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

void DetermineBestMoves()
{
	ifstream counterFile("CurrentCounter.txt");
	unsigned long int currentCounter = 0;
	counterFile >> currentCounter;
	counterFile.close();
	
	unsigned long int iterator = 0;
	for (int i1 = 0; i1 < 48; ++i1)
		for (int i2 = i1 + 1; i2 < 49; ++i2)
			for (int i3 = i2 + 1; i3 < 50; ++i3)
				for (int i4 = i3 + 1; i4 < 51; ++i4)
					for (int i5 = i4 + 1; i5 < 52; ++i5)
					{
						if (iterator++ < currentCounter) continue;

						double averagePay = 0.0f;
						River newRiver(i1, i2, i3, i4, i5);
						HandBestMoveData[currentCounter] = DetermineBestMove(newRiver, averagePay);
						
						//outputFile.write((const char*)(&HandBestMoveData[currentCounter]), sizeof(char));
						
						ofstream outputFile("BestPlay.dat", ((currentCounter == 0) ? ios_base::trunc : ios_base::app) | ios_base::binary);
						outputFile.seekp(0, outputFile.end);
						outputFile.write((const char*)(&i1), sizeof(int));
						outputFile.write((const char*)(&i2), sizeof(int));
						outputFile.write((const char*)(&i3), sizeof(int));
						outputFile.write((const char*)(&i4), sizeof(int));
						outputFile.write((const char*)(&i5), sizeof(int));
						outputFile.write((const char*)(&HandBestMoveData[currentCounter]), sizeof(char));
						outputFile.close();

						cout << "The best move for hand #" << currentCounter + 1 << " - " << DescribeHand(i1, i2, i3, i4, i5) << " is " << DescribeMove(HandBestMoveData[currentCounter]) << std::endl;
						
						ofstream counterFile("CurrentCounter.txt");
						counterFile.clear();
						counterFile << ++currentCounter;
						counterFile.close();
					}

    OutputStatistics();
}

void main()
{
    InitializeStatistics();
	LoadDataFile();
	DetermineBestMoves();
}