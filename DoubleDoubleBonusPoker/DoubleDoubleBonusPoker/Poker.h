#include <string>
#include <stdio.h>
#include <vector>
#include <algorithm>

int GetSuit(int card)   { return (card / 13); }
int GetFace(int card)   { return (card % 13); }

#define HAND_TYPE_COUNT 14
std::pair<std::string, int> WinningHands[HAND_TYPE_COUNT];
int WinningValue[HAND_TYPE_COUNT];

typedef bool (*HandCheckType)(int i1, int i2, int i3, int i4, int i5);
std::vector<HandCheckType> HandCheckList;

#define MAX_RIVER_LAYOUTS 2598960
double HandValueData[MAX_RIVER_LAYOUTS];
char HandBestMoveData[MAX_RIVER_LAYOUTS]; // Bitflag for hold or muck on each of the five cards

char Move(bool b1, bool b2, bool b3, bool b4, bool b5)
{
	return (b1 ? 1 : 0) + (b2 ? 2 : 0) + (b3 ? 4 : 0) + (b4 ? 8 : 0) + (b5 ? 16 : 0);
}

char FaceCharacters[13] = { '2', '3', '4', '5', '6', '7', '8' ,'9', 'T', 'J', 'Q', 'K', 'A' };
char SuitCharacters[4] = { 'D', 'S', 'C', 'H' };

std::string DescribeHand(int c1, int c2, int c3, int c4, int c5)
{
	std::string description = "";
	description += FaceCharacters[c1 % 13];
	description += SuitCharacters[c1 / 13];
	description += " ";
	description += FaceCharacters[c2 % 13];
	description += SuitCharacters[c2 / 13];
	description += " ";
	description += FaceCharacters[c3 % 13];
	description += SuitCharacters[c3 / 13];
	description += " ";
	description += FaceCharacters[c4 % 13];
	description += SuitCharacters[c4 / 13];
	description += " ";
	description += FaceCharacters[c5 % 13];
	description += SuitCharacters[c5 / 13];
	return description;
}

std::string DescribeMove(char move)
{
	std::string description = "";
	description += ((move & 1) ? "H" : "X");
	description += ((move & 2) ? "H" : "X");
	description += ((move & 4) ? "H" : "X");
	description += ((move & 8) ? "H" : "X");
	description += ((move & 16) ? "H" : "X");
	return description;
}

struct River
{
	River(int c1, int c2, int c3, int c4, int c5) : 
		Card1(c1),
		Card2(c2),
		Card3(c3),
		Card4(c4),
		Card5(c5)
	{}

	int Card1;
	int Card2;
	int Card3;
	int Card4;
	int Card5;
};

typedef double (*HandPotentialCheck)(int i1, int i2, int i3, int i4, int i5);
typedef std::vector< std::pair<HandPotentialCheck, char> > HandPotentialCheckListType;
HandPotentialCheckListType HandPotentialCheckList;

bool IsFourAceWithTwoThreeFour(int i1, int i2, int i3, int i4, int i5)
{
	int aceCount = 0;
	int twothreefourCount = 0;
	if (GetFace(i1) == 12) aceCount += 1;
	if (GetFace(i2) == 12) aceCount += 1;
	if (GetFace(i3) == 12) aceCount += 1;
	if (GetFace(i4) == 12) aceCount += 1;
	if (GetFace(i5) == 12) aceCount += 1;
	
	if ((GetFace(i1) == 0) || (GetFace(i1) == 1) || (GetFace(i1) == 2)) twothreefourCount += 1;
	if ((GetFace(i2) == 0) || (GetFace(i2) == 1) || (GetFace(i2) == 2)) twothreefourCount += 1;
	if ((GetFace(i3) == 0) || (GetFace(i3) == 1) || (GetFace(i3) == 2)) twothreefourCount += 1;
	if ((GetFace(i4) == 0) || (GetFace(i4) == 1) || (GetFace(i4) == 2)) twothreefourCount += 1;
	if ((GetFace(i5) == 0) || (GetFace(i5) == 1) || (GetFace(i5) == 2)) twothreefourCount += 1;

	return (aceCount == 4 && twothreefourCount == 1);
}

bool IsRoyalFlush(int i1, int i2, int i3, int i4, int i5)
{
	int suit = GetSuit(i1);
	if (suit != GetSuit(i2)) return false;
	if (suit != GetSuit(i3)) return false;
	if (suit != GetSuit(i4)) return false;
	if (suit != GetSuit(i5)) return false;

    if (GetFace(i1) != 8) return false;
    if (GetFace(i2) != 9) return false;
    if (GetFace(i3) != 10) return false;
    if (GetFace(i4) != 11) return false;
    if (GetFace(i5) != 12) return false; // NOTE: 12 is Ace
    
    return true;
}

bool IsFourTwosThreesFoursWithAce(int i1, int i2, int i3, int i4, int i5)
{
	int twoCount = 0;
	int threeCount = 0;
	int fourCount = 0;
	int aceCount = 0;
	
	if (GetFace(i1) == 0) twoCount += 1;
	if (GetFace(i2) == 0) twoCount += 1;
	if (GetFace(i3) == 0) twoCount += 1;
	if (GetFace(i4) == 0) twoCount += 1;
	if (GetFace(i5) == 0) twoCount += 1;
	
	if (GetFace(i1) == 1) threeCount += 1;
	if (GetFace(i2) == 1) threeCount += 1;
	if (GetFace(i3) == 1) threeCount += 1;
	if (GetFace(i4) == 1) threeCount += 1;
	if (GetFace(i5) == 1) threeCount += 1;

	if (GetFace(i1) == 2) fourCount += 1;
	if (GetFace(i2) == 2) fourCount += 1;
	if (GetFace(i3) == 2) fourCount += 1;
	if (GetFace(i4) == 2) fourCount += 1;
	if (GetFace(i5) == 2) fourCount += 1;

	if (GetFace(i1) == 12) aceCount += 1;
	if (GetFace(i2) == 12) aceCount += 1;
	if (GetFace(i3) == 12) aceCount += 1;
	if (GetFace(i4) == 12) aceCount += 1;
	if (GetFace(i5) == 12) aceCount += 1;

	bool fourTwosThreeFours = ((twoCount == 4) || (threeCount == 4) || (fourCount == 4));
	return (fourTwosThreeFours && (aceCount == 1));
}

bool IsFourAces(int i1, int i2, int i3, int i4, int i5)
{
	int aceCount = 0;

	if (GetFace(i1) == 12) aceCount += 1;
	if (GetFace(i2) == 12) aceCount += 1;
	if (GetFace(i3) == 12) aceCount += 1;
	if (GetFace(i4) == 12) aceCount += 1;
	if (GetFace(i5) == 12) aceCount += 1;

	return (aceCount == 4);
}

bool IsStraightFlush(int i1, int i2, int i3, int i4, int i5)
{
	int suit = GetSuit(i1);
	if (suit != GetSuit(i2)) return false;
	if (suit != GetSuit(i3)) return false;
	if (suit != GetSuit(i4)) return false;
	if (suit != GetSuit(i5)) return false;

	if (GetFace(i1) != GetFace(i2) - 1) return false;
	if (GetFace(i2) != GetFace(i3) - 1) return false;
	if (GetFace(i3) != GetFace(i4) - 1) return false;
	if (GetFace(i4) != GetFace(i5) - 1) return false;

	return true;
}

bool IsFourTwosThreesFours(int i1, int i2, int i3, int i4, int i5)
{
	int twoCount = 0;
	int threeCount = 0;
	int fourCount = 0;
	
	if (GetFace(i1) == 0) twoCount += 1;
	if (GetFace(i2) == 0) twoCount += 1;
	if (GetFace(i3) == 0) twoCount += 1;
	if (GetFace(i4) == 0) twoCount += 1;
	if (GetFace(i5) == 0) twoCount += 1;
	
	if (GetFace(i1) == 1) threeCount += 1;
	if (GetFace(i2) == 1) threeCount += 1;
	if (GetFace(i3) == 1) threeCount += 1;
	if (GetFace(i4) == 1) threeCount += 1;
	if (GetFace(i5) == 1) threeCount += 1;

	if (GetFace(i1) == 2) fourCount += 1;
	if (GetFace(i2) == 2) fourCount += 1;
	if (GetFace(i3) == 2) fourCount += 1;
	if (GetFace(i4) == 2) fourCount += 1;
	if (GetFace(i5) == 2) fourCount += 1;

	return ((twoCount == 4) || (threeCount == 4) || (fourCount == 4));
}

bool IsFourFivesThruKings(int i1, int i2, int i3, int i4, int i5)
{
	int counts[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	for (int i = 0; i < 9; ++i)
	{
		if (GetFace(i1) == i + 3) counts[i] += 1;
		if (GetFace(i2) == i + 3) counts[i] += 1;
		if (GetFace(i3) == i + 3) counts[i] += 1;
		if (GetFace(i4) == i + 3) counts[i] += 1;
		if (GetFace(i5) == i + 3) counts[i] += 1;

		if (counts[i] == 4) return true;
	}
	
	return false;
}

bool IsFullHouse(int i1, int i2, int i3, int i4, int i5)
{
	int counts[13] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	
	for (int i = 0; i < 13; ++i)
	{
		if (GetFace(i1) == i) counts[i] += 1;
		if (GetFace(i2) == i) counts[i] += 1;
		if (GetFace(i3) == i) counts[i] += 1;
		if (GetFace(i4) == i) counts[i] += 1;
		if (GetFace(i5) == i) counts[i] += 1;
	}

	bool two = false;
	bool three = false;
	for (int i = 0; i < 13; ++i)
	{
		two		|= (counts[i] == 2);
		three	|= (counts[i] == 3);
	}

	return (two && three);
}

bool IsFlush(int i1, int i2, int i3, int i4, int i5)
{
	int suit = GetSuit(i1);
	if (suit != GetSuit(i2)) return false;
	if (suit != GetSuit(i3)) return false;
	if (suit != GetSuit(i4)) return false;
	if (suit != GetSuit(i5)) return false;

	return true;
}

bool IsStraight(int i1, int i2, int i3, int i4, int i5)
{
	if (GetFace(i1) != GetFace(i2) - 1) return false;
	if (GetFace(i2) != GetFace(i3) - 1) return false;
	if (GetFace(i3) != GetFace(i4) - 1) return false;
	if (GetFace(i4) != GetFace(i5) - 1) return false;

	return true;
}

bool IsThreeOfAKind(int i1, int i2, int i3, int i4, int i5)
{
	int counts[13] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	
	counts[GetFace(i1)] += 1;
	counts[GetFace(i2)] += 1;
	counts[GetFace(i3)] += 1;
	counts[GetFace(i4)] += 1;
	counts[GetFace(i5)] += 1;

	for (int i = 0; i < 13; ++i)
		if (counts[i] == 3) return true;

	return false;
}

bool IsTwoPair(int i1, int i2, int i3, int i4, int i5)
{
	int counts[13] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	
	counts[GetFace(i1)] += 1;
	counts[GetFace(i2)] += 1;
	counts[GetFace(i3)] += 1;
	counts[GetFace(i4)] += 1;
	counts[GetFace(i5)] += 1;

	int pairs = 0;
	for (int i = 0; i < 13; ++i)
		if (counts[i] == 2)
			pairs += 1;

	return (pairs == 2);
}

bool IsJacksOrBetter(int i1, int i2, int i3, int i4, int i5)
{
	int counts[13] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	
	counts[GetFace(i1)] += 1;
	counts[GetFace(i2)] += 1;
	counts[GetFace(i3)] += 1;
	counts[GetFace(i4)] += 1;
	counts[GetFace(i5)] += 1;

	return ((counts[9] == 2) || (counts[10] == 2) || (counts[11] == 2) || (counts[12] == 2));
}

bool IsNothing(int i1, int i2, int i3, int i4, int i5)
{
	return true;
}

//  NOTE: This function (and this function only) does not require the card indices to be in ascending order
double CheckHand(int i1, int i2, int i3, int i4, int i5)
{
	//std::vector<int> hand;
	//hand.push_back(i1);
	//hand.push_back(i2);
	//hand.push_back(i3);
	//hand.push_back(i4);
	//hand.push_back(i5);
	//std::sort(hand.begin(), hand.end());

	for (int i = 0; i < HAND_TYPE_COUNT; ++i)
	{
		//if ((*HandCheckList[i])(hand[0], hand[1], hand[2], hand[3], hand[4]))
		if ((*HandCheckList[i])(i1, i2, i3, i4, i5))
		{
			WinningHands[i].second += 1;
			return double(WinningValue[i]);
		}
	}

	return 0.0;
}

double CheckPotentialXHHHH(int c1, int c2, int c3, int c4, int c5)
{
	std::vector<int> potentials1;
	for (int i = 0; i < 52; ++i)
	{
		if (i == c1) continue;
		if (i == c2) continue;
		if (i == c3) continue;
		if (i == c4) continue;
		if (i == c5) continue;
		potentials1.push_back(i);
	}

	double totalReturn = 0.0;
	double totalChecks = 0.0;
	for (std::vector<int>::iterator iter1 = potentials1.begin(); iter1 != potentials1.end(); ++iter1)
	{
		totalChecks += 1.0;
		totalReturn += CheckHand((*iter1), c2, c3, c4, c5);
	}

	return totalReturn / totalChecks;
}

double CheckPotentialHXHHH(int c1, int c2, int c3, int c4, int c5)
{
	std::vector<int> potentials1;
	for (int i = 0; i < 52; ++i)
	{
		if (i == c1) continue;
		if (i == c2) continue;
		if (i == c3) continue;
		if (i == c4) continue;
		if (i == c5) continue;
		potentials1.push_back(i);
	}

	double totalReturn = 0.0;
	double totalChecks = 0.0;
	for (std::vector<int>::iterator iter1 = potentials1.begin(); iter1 != potentials1.end(); ++iter1)
	{
		totalChecks += 1.0;
		totalReturn += CheckHand(c1, (*iter1), c3, c4, c5);
	}

	return totalReturn / totalChecks;
}

double CheckPotentialHHXHH(int c1, int c2, int c3, int c4, int c5)
{
	std::vector<int> potentials1;
	for (int i = 0; i < 52; ++i)
	{
		if (i == c1) continue;
		if (i == c2) continue;
		if (i == c3) continue;
		if (i == c4) continue;
		if (i == c5) continue;
		potentials1.push_back(i);
	}

	double totalReturn = 0.0;
	double totalChecks = 0.0;
	for (std::vector<int>::iterator iter1 = potentials1.begin(); iter1 != potentials1.end(); ++iter1)
	{
		totalChecks += 1.0;
		totalReturn += CheckHand(c1, c2, (*iter1), c4, c5);
	}

	return totalReturn / totalChecks;
}

double CheckPotentialHHHXH(int c1, int c2, int c3, int c4, int c5)
{
	std::vector<int> potentials1;
	for (int i = 0; i < 52; ++i)
	{
		if (i == c1) continue;
		if (i == c2) continue;
		if (i == c3) continue;
		if (i == c4) continue;
		if (i == c5) continue;
		potentials1.push_back(i);
	}

	double totalReturn = 0.0;
	double totalChecks = 0.0;
	for (std::vector<int>::iterator iter1 = potentials1.begin(); iter1 != potentials1.end(); ++iter1)
	{
		totalChecks += 1.0;
		totalReturn += CheckHand(c1, c2, c3, (*iter1), c5);
	}

	return totalReturn / totalChecks;
}

double CheckPotentialHHHHX(int c1, int c2, int c3, int c4, int c5)
{
	std::vector<int> potentials1;
	for (int i = 0; i < 52; ++i)
	{
		if (i == c1) continue;
		if (i == c2) continue;
		if (i == c3) continue;
		if (i == c4) continue;
		if (i == c5) continue;
		potentials1.push_back(i);
	}

	double totalReturn = 0.0;
	double totalChecks = 0.0;
	for (std::vector<int>::iterator iter1 = potentials1.begin(); iter1 != potentials1.end(); ++iter1)
	{
		totalChecks += 1.0;
		totalReturn += CheckHand(c1, c2, c3, c4, (*iter1));
	}

	return totalReturn / totalChecks;
}

double CheckPotentialXXHHH(int c1, int c2, int c3, int c4, int c5)
{
	std::vector<int> potentials1;
	for (int i = 0; i < 52; ++i)
	{
		if (i == c1) continue;
		if (i == c2) continue;
		if (i == c3) continue;
		if (i == c4) continue;
		if (i == c5) continue;
		potentials1.push_back(i);
	}
	
	std::vector<int> potentials2 = potentials1;

	double totalReturn = 0.0;
	double totalChecks = 0.0;
	for (std::vector<int>::iterator iter1 = potentials1.begin(); iter1 != potentials1.end(); ++iter1)
	{
		for (std::vector<int>::iterator iter2 = potentials2.begin(); iter2 != potentials2.end(); ++iter2)
		{
			if ((*iter2) <= (*iter1)) continue;

			totalChecks += 1.0;
			totalReturn += CheckHand((*iter1), (*iter2), c3, c4, c5);
		}
	}

	return totalReturn / totalChecks;
}

double CheckPotentialXHXHH(int c1, int c2, int c3, int c4, int c5)
{
	std::vector<int> potentials1;
	for (int i = 0; i < 52; ++i)
	{
		if (i == c1) continue;
		if (i == c2) continue;
		if (i == c3) continue;
		if (i == c4) continue;
		if (i == c5) continue;
		potentials1.push_back(i);
	}
	
	std::vector<int> potentials2 = potentials1;

	double totalReturn = 0.0;
	double totalChecks = 0.0;
	for (std::vector<int>::iterator iter1 = potentials1.begin(); iter1 != potentials1.end(); ++iter1)
	{
		for (std::vector<int>::iterator iter2 = potentials2.begin(); iter2 != potentials2.end(); ++iter2)
		{
			if ((*iter2) <= (*iter1)) continue;

			totalChecks += 1.0;
			totalReturn += CheckHand((*iter1), c2, (*iter2), c4, c5);
		}
	}

	return totalReturn / totalChecks;
}

double CheckPotentialXHHXH(int c1, int c2, int c3, int c4, int c5)
{
	std::vector<int> potentials1;
	for (int i = 0; i < 52; ++i)
	{
		if (i == c1) continue;
		if (i == c2) continue;
		if (i == c3) continue;
		if (i == c4) continue;
		if (i == c5) continue;
		potentials1.push_back(i);
	}
	
	std::vector<int> potentials2 = potentials1;

	double totalReturn = 0.0;
	double totalChecks = 0.0;
	for (std::vector<int>::iterator iter1 = potentials1.begin(); iter1 != potentials1.end(); ++iter1)
	{
		for (std::vector<int>::iterator iter2 = potentials2.begin(); iter2 != potentials2.end(); ++iter2)
		{
			if ((*iter2) <= (*iter1)) continue;

			totalChecks += 1.0;
			totalReturn += CheckHand((*iter1), c2, c3, (*iter2), c5);
		}
	}

	return totalReturn / totalChecks;
}

double CheckPotentialXHHHX(int c1, int c2, int c3, int c4, int c5)
{
	std::vector<int> potentials1;
	for (int i = 0; i < 52; ++i)
	{
		if (i == c1) continue;
		if (i == c2) continue;
		if (i == c3) continue;
		if (i == c4) continue;
		if (i == c5) continue;
		potentials1.push_back(i);
	}
	
	std::vector<int> potentials2 = potentials1;

	double totalReturn = 0.0;
	double totalChecks = 0.0;
	for (std::vector<int>::iterator iter1 = potentials1.begin(); iter1 != potentials1.end(); ++iter1)
	{
		for (std::vector<int>::iterator iter2 = potentials2.begin(); iter2 != potentials2.end(); ++iter2)
		{
			if ((*iter2) <= (*iter1)) continue;

			totalChecks += 1.0;
			totalReturn += CheckHand((*iter1), c2, c3, c4, (*iter2));
		}
	}

	return totalReturn / totalChecks;
}

double CheckPotentialHXXHH(int c1, int c2, int c3, int c4, int c5)
{
	std::vector<int> potentials1;
	for (int i = 0; i < 52; ++i)
	{
		if (i == c1) continue;
		if (i == c2) continue;
		if (i == c3) continue;
		if (i == c4) continue;
		if (i == c5) continue;
		potentials1.push_back(i);
	}
	
	std::vector<int> potentials2 = potentials1;

	double totalReturn = 0.0;
	double totalChecks = 0.0;
	for (std::vector<int>::iterator iter1 = potentials1.begin(); iter1 != potentials1.end(); ++iter1)
	{
		for (std::vector<int>::iterator iter2 = potentials2.begin(); iter2 != potentials2.end(); ++iter2)
		{
			if ((*iter2) <= (*iter1)) continue;

			totalChecks += 1.0;
			totalReturn += CheckHand(c1, (*iter1), (*iter2), c4, c5);
		}
	}

	return totalReturn / totalChecks;
}

double CheckPotentialHXHXH(int c1, int c2, int c3, int c4, int c5)
{
	std::vector<int> potentials1;
	for (int i = 0; i < 52; ++i)
	{
		if (i == c1) continue;
		if (i == c2) continue;
		if (i == c3) continue;
		if (i == c4) continue;
		if (i == c5) continue;
		potentials1.push_back(i);
	}
	
	std::vector<int> potentials2 = potentials1;

	double totalReturn = 0.0;
	double totalChecks = 0.0;
	for (std::vector<int>::iterator iter1 = potentials1.begin(); iter1 != potentials1.end(); ++iter1)
	{
		for (std::vector<int>::iterator iter2 = potentials2.begin(); iter2 != potentials2.end(); ++iter2)
		{
			if ((*iter2) <= (*iter1)) continue;

			totalChecks += 1.0;
			totalReturn += CheckHand(c1, (*iter1), c3, (*iter2), c5);
		}
	}

	return totalReturn / totalChecks;
}

double CheckPotentialHXHHX(int c1, int c2, int c3, int c4, int c5)
{
	std::vector<int> potentials1;
	for (int i = 0; i < 52; ++i)
	{
		if (i == c1) continue;
		if (i == c2) continue;
		if (i == c3) continue;
		if (i == c4) continue;
		if (i == c5) continue;
		potentials1.push_back(i);
	}
	
	std::vector<int> potentials2 = potentials1;

	double totalReturn = 0.0;
	double totalChecks = 0.0;
	for (std::vector<int>::iterator iter1 = potentials1.begin(); iter1 != potentials1.end(); ++iter1)
	{
		for (std::vector<int>::iterator iter2 = potentials2.begin(); iter2 != potentials2.end(); ++iter2)
		{
			if ((*iter2) <= (*iter1)) continue;

			totalChecks += 1.0;
			totalReturn += CheckHand(c1, (*iter1), c3, c4, (*iter2));
		}
	}

	return totalReturn / totalChecks;
}

double CheckPotentialHHXXH(int c1, int c2, int c3, int c4, int c5)
{
	std::vector<int> potentials1;
	for (int i = 0; i < 52; ++i)
	{
		if (i == c1) continue;
		if (i == c2) continue;
		if (i == c3) continue;
		if (i == c4) continue;
		if (i == c5) continue;
		potentials1.push_back(i);
	}
	
	std::vector<int> potentials2 = potentials1;

	double totalReturn = 0.0;
	double totalChecks = 0.0;
	for (std::vector<int>::iterator iter1 = potentials1.begin(); iter1 != potentials1.end(); ++iter1)
	{
		for (std::vector<int>::iterator iter2 = potentials2.begin(); iter2 != potentials2.end(); ++iter2)
		{
			if ((*iter2) <= (*iter1)) continue;

			totalChecks += 1.0;
			totalReturn += CheckHand(c1, c2, (*iter1), (*iter2), c5);
		}
	}

	return totalReturn / totalChecks;
}

double CheckPotentialHHXHX(int c1, int c2, int c3, int c4, int c5)
{
	std::vector<int> potentials1;
	for (int i = 0; i < 52; ++i)
	{
		if (i == c1) continue;
		if (i == c2) continue;
		if (i == c3) continue;
		if (i == c4) continue;
		if (i == c5) continue;
		potentials1.push_back(i);
	}
	
	std::vector<int> potentials2 = potentials1;

	double totalReturn = 0.0;
	double totalChecks = 0.0;
	for (std::vector<int>::iterator iter1 = potentials1.begin(); iter1 != potentials1.end(); ++iter1)
	{
		for (std::vector<int>::iterator iter2 = potentials2.begin(); iter2 != potentials2.end(); ++iter2)
		{
			if ((*iter2) <= (*iter1)) continue;

			totalChecks += 1.0;
			totalReturn += CheckHand(c1, c2, (*iter1), c4, (*iter2));
		}
	}

	return totalReturn / totalChecks;
}

double CheckPotentialHHHXX(int c1, int c2, int c3, int c4, int c5)
{
	std::vector<int> potentials1;
	for (int i = 0; i < 52; ++i)
	{
		if (i == c1) continue;
		if (i == c2) continue;
		if (i == c3) continue;
		if (i == c4) continue;
		if (i == c5) continue;
		potentials1.push_back(i);
	}
	
	std::vector<int> potentials2 = potentials1;

	double totalReturn = 0.0;
	double totalChecks = 0.0;
	for (std::vector<int>::iterator iter1 = potentials1.begin(); iter1 != potentials1.end(); ++iter1)
	{
		for (std::vector<int>::iterator iter2 = potentials2.begin(); iter2 != potentials2.end(); ++iter2)
		{
			if ((*iter2) <= (*iter1)) continue;

			totalChecks += 1.0;
			totalReturn += CheckHand(c1, c2, c3, (*iter1), (*iter2));
		}
	}

	return totalReturn / totalChecks;
}

double CheckPotentialXXXHH(int c1, int c2, int c3, int c4, int c5)
{
	std::vector<int> potentials1;
	for (int i = 0; i < 52; ++i)
	{
		if (i == c1) continue;
		if (i == c2) continue;
		if (i == c3) continue;
		if (i == c4) continue;
		if (i == c5) continue;
		potentials1.push_back(i);
	}
	
	std::vector<int> potentials2 = potentials1;
	std::vector<int> potentials3 = potentials1;

	double totalReturn = 0.0;
	double totalChecks = 0.0;
	for (std::vector<int>::iterator iter1 = potentials1.begin(); iter1 != potentials1.end(); ++iter1)
	{
		for (std::vector<int>::iterator iter2 = potentials2.begin(); iter2 != potentials2.end(); ++iter2)
		{
			if ((*iter2) <= (*iter1)) continue;
			for (std::vector<int>::iterator iter3 = potentials3.begin(); iter3 != potentials3.end(); ++iter3)
			{
				if ((*iter3) <= (*iter2)) continue;

				totalChecks += 1.0;
				totalReturn += CheckHand((*iter1), (*iter2), (*iter3), c4, c5);
			}
		}
	}

	return totalReturn / totalChecks;
}

double CheckPotentialXXHXH(int c1, int c2, int c3, int c4, int c5)
{
	std::vector<int> potentials1;
	for (int i = 0; i < 52; ++i)
	{
		if (i == c1) continue;
		if (i == c2) continue;
		if (i == c3) continue;
		if (i == c4) continue;
		if (i == c5) continue;
		potentials1.push_back(i);
	}
	
	std::vector<int> potentials2 = potentials1;
	std::vector<int> potentials3 = potentials1;

	double totalReturn = 0.0;
	double totalChecks = 0.0;
	for (std::vector<int>::iterator iter1 = potentials1.begin(); iter1 != potentials1.end(); ++iter1)
	{
		for (std::vector<int>::iterator iter2 = potentials2.begin(); iter2 != potentials2.end(); ++iter2)
		{
			if ((*iter2) <= (*iter1)) continue;
			for (std::vector<int>::iterator iter3 = potentials3.begin(); iter3 != potentials3.end(); ++iter3)
			{
				if ((*iter3) <= (*iter2)) continue;

				totalChecks += 1.0;
				totalReturn += CheckHand((*iter1), (*iter2), c3, (*iter3), c5);
			}
		}
	}

	return totalReturn / totalChecks;
}

double CheckPotentialXXHHX(int c1, int c2, int c3, int c4, int c5)
{
	std::vector<int> potentials1;
	for (int i = 0; i < 52; ++i)
	{
		if (i == c1) continue;
		if (i == c2) continue;
		if (i == c3) continue;
		if (i == c4) continue;
		if (i == c5) continue;
		potentials1.push_back(i);
	}
	
	std::vector<int> potentials2 = potentials1;
	std::vector<int> potentials3 = potentials1;

	double totalReturn = 0.0;
	double totalChecks = 0.0;
	for (std::vector<int>::iterator iter1 = potentials1.begin(); iter1 != potentials1.end(); ++iter1)
	{
		for (std::vector<int>::iterator iter2 = potentials2.begin(); iter2 != potentials2.end(); ++iter2)
		{
			if ((*iter2) <= (*iter1)) continue;
			for (std::vector<int>::iterator iter3 = potentials3.begin(); iter3 != potentials3.end(); ++iter3)
			{
				if ((*iter3) <= (*iter2)) continue;

				totalChecks += 1.0;
				totalReturn += CheckHand((*iter1), (*iter2), c3, c4, (*iter3));
			}
		}
	}

	return totalReturn / totalChecks;
}

double CheckPotentialXHXXH(int c1, int c2, int c3, int c4, int c5)
{
	std::vector<int> potentials1;
	for (int i = 0; i < 52; ++i)
	{
		if (i == c1) continue;
		if (i == c2) continue;
		if (i == c3) continue;
		if (i == c4) continue;
		if (i == c5) continue;
		potentials1.push_back(i);
	}
	
	std::vector<int> potentials2 = potentials1;
	std::vector<int> potentials3 = potentials1;

	double totalReturn = 0.0;
	double totalChecks = 0.0;
	for (std::vector<int>::iterator iter1 = potentials1.begin(); iter1 != potentials1.end(); ++iter1)
	{
		for (std::vector<int>::iterator iter2 = potentials2.begin(); iter2 != potentials2.end(); ++iter2)
		{
			if ((*iter2) <= (*iter1)) continue;
			for (std::vector<int>::iterator iter3 = potentials3.begin(); iter3 != potentials3.end(); ++iter3)
			{
				if ((*iter3) <= (*iter2)) continue;

				totalChecks += 1.0;
				totalReturn += CheckHand((*iter1), c2, (*iter2), (*iter3), c5);
			}
		}
	}

	return totalReturn / totalChecks;
}

double CheckPotentialXHXHX(int c1, int c2, int c3, int c4, int c5)
{
	std::vector<int> potentials1;
	for (int i = 0; i < 52; ++i)
	{
		if (i == c1) continue;
		if (i == c2) continue;
		if (i == c3) continue;
		if (i == c4) continue;
		if (i == c5) continue;
		potentials1.push_back(i);
	}
	
	std::vector<int> potentials2 = potentials1;
	std::vector<int> potentials3 = potentials1;

	double totalReturn = 0.0;
	double totalChecks = 0.0;
	for (std::vector<int>::iterator iter1 = potentials1.begin(); iter1 != potentials1.end(); ++iter1)
	{
		for (std::vector<int>::iterator iter2 = potentials2.begin(); iter2 != potentials2.end(); ++iter2)
		{
			if ((*iter2) <= (*iter1)) continue;
			for (std::vector<int>::iterator iter3 = potentials3.begin(); iter3 != potentials3.end(); ++iter3)
			{
				if ((*iter3) <= (*iter2)) continue;

				totalChecks += 1.0;
				totalReturn += CheckHand((*iter1), c2, (*iter2), c4, (*iter3));
			}
		}
	}

	return totalReturn / totalChecks;
}

double CheckPotentialXHHXX(int c1, int c2, int c3, int c4, int c5)
{
	std::vector<int> potentials1;
	for (int i = 0; i < 52; ++i)
	{
		if (i == c1) continue;
		if (i == c2) continue;
		if (i == c3) continue;
		if (i == c4) continue;
		if (i == c5) continue;
		potentials1.push_back(i);
	}
	
	std::vector<int> potentials2 = potentials1;
	std::vector<int> potentials3 = potentials1;

	double totalReturn = 0.0;
	double totalChecks = 0.0;
	for (std::vector<int>::iterator iter1 = potentials1.begin(); iter1 != potentials1.end(); ++iter1)
	{
		for (std::vector<int>::iterator iter2 = potentials2.begin(); iter2 != potentials2.end(); ++iter2)
		{
			if ((*iter2) <= (*iter1)) continue;
			for (std::vector<int>::iterator iter3 = potentials3.begin(); iter3 != potentials3.end(); ++iter3)
			{
				if ((*iter3) <= (*iter2)) continue;

				totalChecks += 1.0;
				totalReturn += CheckHand((*iter1), c2, c3, (*iter2), (*iter3));
			}
		}
	}

	return totalReturn / totalChecks;
}

double CheckPotentialHXXXH(int c1, int c2, int c3, int c4, int c5)
{
	std::vector<int> potentials1;
	for (int i = 0; i < 52; ++i)
	{
		if (i == c1) continue;
		if (i == c2) continue;
		if (i == c3) continue;
		if (i == c4) continue;
		if (i == c5) continue;
		potentials1.push_back(i);
	}
	
	std::vector<int> potentials2 = potentials1;
	std::vector<int> potentials3 = potentials1;

	double totalReturn = 0.0;
	double totalChecks = 0.0;
	for (std::vector<int>::iterator iter1 = potentials1.begin(); iter1 != potentials1.end(); ++iter1)
	{
		for (std::vector<int>::iterator iter2 = potentials2.begin(); iter2 != potentials2.end(); ++iter2)
		{
			if ((*iter2) <= (*iter1)) continue;
			for (std::vector<int>::iterator iter3 = potentials3.begin(); iter3 != potentials3.end(); ++iter3)
			{
				if ((*iter3) <= (*iter2)) continue;

				totalChecks += 1.0;
				totalReturn += CheckHand(c1, (*iter1), (*iter2), (*iter3), c5);
			}
		}
	}

	return totalReturn / totalChecks;
}

double CheckPotentialHXXHX(int c1, int c2, int c3, int c4, int c5)
{
	std::vector<int> potentials1;
	for (int i = 0; i < 52; ++i)
	{
		if (i == c1) continue;
		if (i == c2) continue;
		if (i == c3) continue;
		if (i == c4) continue;
		if (i == c5) continue;
		potentials1.push_back(i);
	}
	
	std::vector<int> potentials2 = potentials1;
	std::vector<int> potentials3 = potentials1;

	double totalReturn = 0.0;
	double totalChecks = 0.0;
	for (std::vector<int>::iterator iter1 = potentials1.begin(); iter1 != potentials1.end(); ++iter1)
	{
		for (std::vector<int>::iterator iter2 = potentials2.begin(); iter2 != potentials2.end(); ++iter2)
		{
			if ((*iter2) <= (*iter1)) continue;
			for (std::vector<int>::iterator iter3 = potentials3.begin(); iter3 != potentials3.end(); ++iter3)
			{
				if ((*iter3) <= (*iter2)) continue;

				totalChecks += 1.0;
				totalReturn += CheckHand(c1, (*iter1), (*iter2), c4, (*iter3));
			}
		}
	}

	return totalReturn / totalChecks;
}

double CheckPotentialHXHXX(int c1, int c2, int c3, int c4, int c5)
{
	std::vector<int> potentials1;
	for (int i = 0; i < 52; ++i)
	{
		if (i == c1) continue;
		if (i == c2) continue;
		if (i == c3) continue;
		if (i == c4) continue;
		if (i == c5) continue;
		potentials1.push_back(i);
	}
	
	std::vector<int> potentials2 = potentials1;
	std::vector<int> potentials3 = potentials1;

	double totalReturn = 0.0;
	double totalChecks = 0.0;
	for (std::vector<int>::iterator iter1 = potentials1.begin(); iter1 != potentials1.end(); ++iter1)
	{
		for (std::vector<int>::iterator iter2 = potentials2.begin(); iter2 != potentials2.end(); ++iter2)
		{
			if ((*iter2) <= (*iter1)) continue;
			for (std::vector<int>::iterator iter3 = potentials3.begin(); iter3 != potentials3.end(); ++iter3)
			{
				if ((*iter3) <= (*iter2)) continue;

				totalChecks += 1.0;
				totalReturn += CheckHand(c1, (*iter1), c3, (*iter2), (*iter3));
			}
		}
	}

	return totalReturn / totalChecks;
}

double CheckPotentialHHXXX(int c1, int c2, int c3, int c4, int c5)
{
	std::vector<int> potentials1;
	for (int i = 0; i < 52; ++i)
	{
		if (i == c1) continue;
		if (i == c2) continue;
		if (i == c3) continue;
		if (i == c4) continue;
		if (i == c5) continue;
		potentials1.push_back(i);
	}
	
	std::vector<int> potentials2 = potentials1;
	std::vector<int> potentials3 = potentials1;

	double totalReturn = 0.0;
	double totalChecks = 0.0;
	for (std::vector<int>::iterator iter1 = potentials1.begin(); iter1 != potentials1.end(); ++iter1)
	{
		for (std::vector<int>::iterator iter2 = potentials2.begin(); iter2 != potentials2.end(); ++iter2)
		{
			if ((*iter2) <= (*iter1)) continue;
			for (std::vector<int>::iterator iter3 = potentials3.begin(); iter3 != potentials3.end(); ++iter3)
			{
				if ((*iter3) <= (*iter2)) continue;

				totalChecks += 1.0;
				totalReturn += CheckHand(c1, c2, (*iter1), (*iter2), (*iter3));
			}
		}
	}

	return totalReturn / totalChecks;
}

double CheckPotentialXXXXH(int c1, int c2, int c3, int c4, int c5)
{
	std::vector<int> potentials1;
	for (int i = 0; i < 52; ++i)
	{
		if (i == c1) continue;
		if (i == c2) continue;
		if (i == c3) continue;
		if (i == c4) continue;
		if (i == c5) continue;
		potentials1.push_back(i);
	}
	
	std::vector<int> potentials2 = potentials1;
	std::vector<int> potentials3 = potentials1;
	std::vector<int> potentials4 = potentials1;

	double totalReturn = 0.0;
	double totalChecks = 0.0;
	for (std::vector<int>::iterator iter1 = potentials1.begin(); iter1 != potentials1.end(); ++iter1)
	{
		for (std::vector<int>::iterator iter2 = potentials2.begin(); iter2 != potentials2.end(); ++iter2)
		{
			if ((*iter2) <= (*iter1)) continue;
			for (std::vector<int>::iterator iter3 = potentials3.begin(); iter3 != potentials3.end(); ++iter3)
			{
				if ((*iter3) <= (*iter2)) continue;
				for (std::vector<int>::iterator iter4 = potentials4.begin(); iter4 != potentials4.end(); ++iter4)
				{
					if ((*iter4) <= (*iter3)) continue;

					totalChecks += 1.0;
					totalReturn += CheckHand((*iter1), (*iter2), (*iter3), (*iter4), c5);
				}
			}
		}
	}

	return totalReturn / totalChecks;
}

double CheckPotentialXXXHX(int c1, int c2, int c3, int c4, int c5)
{
	std::vector<int> potentials1;
	for (int i = 0; i < 52; ++i)
	{
		if (i == c1) continue;
		if (i == c2) continue;
		if (i == c3) continue;
		if (i == c4) continue;
		if (i == c5) continue;
		potentials1.push_back(i);
	}
	
	std::vector<int> potentials2 = potentials1;
	std::vector<int> potentials3 = potentials1;
	std::vector<int> potentials4 = potentials1;

	double totalReturn = 0.0;
	double totalChecks = 0.0;
	for (std::vector<int>::iterator iter1 = potentials1.begin(); iter1 != potentials1.end(); ++iter1)
	{
		for (std::vector<int>::iterator iter2 = potentials2.begin(); iter2 != potentials2.end(); ++iter2)
		{
			if ((*iter2) <= (*iter1)) continue;
			for (std::vector<int>::iterator iter3 = potentials3.begin(); iter3 != potentials3.end(); ++iter3)
			{
				if ((*iter3) <= (*iter2)) continue;
				for (std::vector<int>::iterator iter4 = potentials4.begin(); iter4 != potentials4.end(); ++iter4)
				{
					if ((*iter4) <= (*iter3)) continue;

					totalChecks += 1.0;
					totalReturn += CheckHand((*iter1), (*iter2), (*iter3), c4, (*iter4));
				}
			}
		}
	}

	return totalReturn / totalChecks;
}

double CheckPotentialXXHXX(int c1, int c2, int c3, int c4, int c5)
{
	std::vector<int> potentials1;
	for (int i = 0; i < 52; ++i)
	{
		if (i == c1) continue;
		if (i == c2) continue;
		if (i == c3) continue;
		if (i == c4) continue;
		if (i == c5) continue;
		potentials1.push_back(i);
	}
	
	std::vector<int> potentials2 = potentials1;
	std::vector<int> potentials3 = potentials1;
	std::vector<int> potentials4 = potentials1;

	double totalReturn = 0.0;
	double totalChecks = 0.0;
	for (std::vector<int>::iterator iter1 = potentials1.begin(); iter1 != potentials1.end(); ++iter1)
	{
		for (std::vector<int>::iterator iter2 = potentials2.begin(); iter2 != potentials2.end(); ++iter2)
		{
			if ((*iter2) <= (*iter1)) continue;
			for (std::vector<int>::iterator iter3 = potentials3.begin(); iter3 != potentials3.end(); ++iter3)
			{
				if ((*iter3) <= (*iter2)) continue;
				for (std::vector<int>::iterator iter4 = potentials4.begin(); iter4 != potentials4.end(); ++iter4)
				{
					if ((*iter4) <= (*iter3)) continue;

					totalChecks += 1.0;
					totalReturn += CheckHand((*iter1), (*iter2), c3, (*iter3), (*iter4));
				}
			}
		}
	}

	return totalReturn / totalChecks;
}

double CheckPotentialXHXXX(int c1, int c2, int c3, int c4, int c5)
{
	std::vector<int> potentials1;
	for (int i = 0; i < 52; ++i)
	{
		if (i == c1) continue;
		if (i == c2) continue;
		if (i == c3) continue;
		if (i == c4) continue;
		if (i == c5) continue;
		potentials1.push_back(i);
	}
	
	std::vector<int> potentials2 = potentials1;
	std::vector<int> potentials3 = potentials1;
	std::vector<int> potentials4 = potentials1;

	double totalReturn = 0.0;
	double totalChecks = 0.0;
	for (std::vector<int>::iterator iter1 = potentials1.begin(); iter1 != potentials1.end(); ++iter1)
	{
		for (std::vector<int>::iterator iter2 = potentials2.begin(); iter2 != potentials2.end(); ++iter2)
		{
			if ((*iter2) <= (*iter1)) continue;
			for (std::vector<int>::iterator iter3 = potentials3.begin(); iter3 != potentials3.end(); ++iter3)
			{
				if ((*iter3) <= (*iter2)) continue;
				for (std::vector<int>::iterator iter4 = potentials4.begin(); iter4 != potentials4.end(); ++iter4)
				{
					if ((*iter4) <= (*iter3)) continue;

					totalChecks += 1.0;
					totalReturn += CheckHand((*iter1), c2, (*iter2), (*iter3), (*iter4));
				}
			}
		}
	}

	return totalReturn / totalChecks;
}

double CheckPotentialHXXXX(int c1, int c2, int c3, int c4, int c5)
{
	std::vector<int> potentials1;
	for (int i = 0; i < 52; ++i)
	{
		if (i == c1) continue;
		if (i == c2) continue;
		if (i == c3) continue;
		if (i == c4) continue;
		if (i == c5) continue;
		potentials1.push_back(i);
	}
	
	std::vector<int> potentials2 = potentials1;
	std::vector<int> potentials3 = potentials1;
	std::vector<int> potentials4 = potentials1;

	double totalReturn = 0.0;
	double totalChecks = 0.0;
	for (std::vector<int>::iterator iter1 = potentials1.begin(); iter1 != potentials1.end(); ++iter1)
	{
		for (std::vector<int>::iterator iter2 = potentials2.begin(); iter2 != potentials2.end(); ++iter2)
		{
			if ((*iter2) <= (*iter1)) continue;
			for (std::vector<int>::iterator iter3 = potentials3.begin(); iter3 != potentials3.end(); ++iter3)
			{
				if ((*iter3) <= (*iter2)) continue;
				for (std::vector<int>::iterator iter4 = potentials4.begin(); iter4 != potentials4.end(); ++iter4)
				{
					if ((*iter4) <= (*iter3)) continue;

					totalChecks += 1.0;
					totalReturn += CheckHand(c1, (*iter1), (*iter2), (*iter3), (*iter4));
				}
			}
		}
	}

	return totalReturn / totalChecks;
}

double CheckPotentialXXXXX(int c1, int c2, int c3, int c4, int c5)
{
	std::vector<int> potentials1;
	for (int i = 0; i < 52; ++i)
	{
		if (i == c1) continue;
		if (i == c2) continue;
		if (i == c3) continue;
		if (i == c4) continue;
		if (i == c5) continue;
		potentials1.push_back(i);
	}
	
	std::vector<int> potentials2 = potentials1;
	std::vector<int> potentials3 = potentials1;
	std::vector<int> potentials4 = potentials1;
	std::vector<int> potentials5 = potentials1;

	double totalReturn = 0.0;
	double totalChecks = 0.0;
	for (std::vector<int>::iterator iter1 = potentials1.begin(); iter1 != potentials1.end(); ++iter1)
	{
		for (std::vector<int>::iterator iter2 = potentials2.begin(); iter2 != potentials2.end(); ++iter2)
		{
			if ((*iter2) <= (*iter1)) continue;
			for (std::vector<int>::iterator iter3 = potentials3.begin(); iter3 != potentials3.end(); ++iter3)
			{
				if ((*iter3) <= (*iter2)) continue;
				for (std::vector<int>::iterator iter4 = potentials4.begin(); iter4 != potentials4.end(); ++iter4)
				{
					if ((*iter4) <= (*iter3)) continue;
					for (std::vector<int>::iterator iter5 = potentials5.begin(); iter5 != potentials5.end(); ++iter5)
					{
						if ((*iter5) <= (*iter4)) continue;

						totalChecks += 1.0;
						totalReturn += CheckHand((*iter1), (*iter2), (*iter3), (*iter4), (*iter5));
					}
				}
			}
		}
	}

	return totalReturn / totalChecks;
}

void InitializeStatistics()
{
	//  NORMAL BET INFO
	/*
    WinningHands[0]     = std::pair<std::string, int>("4 Aces with any 2,3,4", 0);
    WinningHands[1]     = std::pair<std::string, int>("Royal Flush", 0);
    WinningHands[2]     = std::pair<std::string, int>("4 2s, 3s, 4s with Ace", 0);
    WinningHands[3]     = std::pair<std::string, int>("4 Aces", 0);
    WinningHands[4]     = std::pair<std::string, int>("Straight Flush", 0);
    WinningHands[5]     = std::pair<std::string, int>("4 2-4", 0);
    WinningHands[6]     = std::pair<std::string, int>("4 5-K", 0);
    WinningHands[7]     = std::pair<std::string, int>("Full House", 0);
    WinningHands[8]     = std::pair<std::string, int>("Flush", 0);
    WinningHands[9]     = std::pair<std::string, int>("Straight", 0);
    WinningHands[10]    = std::pair<std::string, int>("Three of a Kind", 0);
    WinningHands[11]    = std::pair<std::string, int>("Two Pair", 0);
    WinningHands[12]    = std::pair<std::string, int>("Jacks or Better", 0);
    WinningHands[13]    = std::pair<std::string, int>("Nothing", 0);
    WinningValue[0]     = 400;
    WinningValue[1]     = 250;
    WinningValue[2]     = 160;
    WinningValue[3]     = 160;
    WinningValue[4]     = 50;
    WinningValue[5]     = 80;
    WinningValue[6]     = 50;
    WinningValue[7]     = 9;
    WinningValue[8]     = 5;
    WinningValue[9]     = 4;
    WinningValue[10]    = 3;
    WinningValue[11]    = 1;
    WinningValue[12]    = 1;
    WinningValue[13]    = 0;
	*/
	
	//  MAX BET INFO
	/**/
    WinningHands[0]     = std::pair<std::string, int>("Royal Flush", 0);
    WinningHands[1]     = std::pair<std::string, int>("4 Aces with any 2,3,4", 0);
    WinningHands[2]     = std::pair<std::string, int>("4 2s, 3s, 4s with Ace", 0);
    WinningHands[3]     = std::pair<std::string, int>("4 Aces", 0);
    WinningHands[5]     = std::pair<std::string, int>("4 2-4", 0);
    WinningHands[4]     = std::pair<std::string, int>("Straight Flush", 0);
    WinningHands[6]     = std::pair<std::string, int>("4 5-K", 0);
    WinningHands[7]     = std::pair<std::string, int>("Full House", 0);
    WinningHands[8]     = std::pair<std::string, int>("Flush", 0);
    WinningHands[9]     = std::pair<std::string, int>("Straight", 0);
    WinningHands[10]    = std::pair<std::string, int>("Three of a Kind", 0);
    WinningHands[11]    = std::pair<std::string, int>("Two Pair", 0);
    WinningHands[12]    = std::pair<std::string, int>("Jacks or Better", 0);
    WinningHands[13]    = std::pair<std::string, int>("Nothing", 0);
    WinningValue[0]     = 4000;
    WinningValue[1]     = 2000;
    WinningValue[2]     = 800;
    WinningValue[3]     = 800;
    WinningValue[4]     = 400;
    WinningValue[5]     = 250;
    WinningValue[6]     = 250;
    WinningValue[7]     = 45;
    WinningValue[8]     = 25;
    WinningValue[9]     = 20;
    WinningValue[10]    = 15;
    WinningValue[11]    = 5;
    WinningValue[12]    = 5;
    WinningValue[13]    = 0;
	/**/

	//  NORMAL BET ORDER
	/*
	HandCheckList.push_back(&IsFourAceWithTwoThreeFour);
	HandCheckList.push_back(&IsRoyalFlush);
	HandCheckList.push_back(&IsFourTwosThreesFoursWithAce);
	HandCheckList.push_back(&IsFourAces);
	HandCheckList.push_back(&IsStraightFlush);
	HandCheckList.push_back(&IsFourTwosThreesFours);
	HandCheckList.push_back(&IsFourFivesThruKings);
	HandCheckList.push_back(&IsFullHouse);
	HandCheckList.push_back(&IsFlush);
	HandCheckList.push_back(&IsStraight);
	HandCheckList.push_back(&IsThreeOfAKind);
	HandCheckList.push_back(&IsTwoPair);
	HandCheckList.push_back(&IsJacksOrBetter);
	HandCheckList.push_back(&IsNothing);
	*/

	//  MAX BET ORDER
	/**/
	HandCheckList.push_back(&IsRoyalFlush);
	HandCheckList.push_back(&IsFourAceWithTwoThreeFour);
	HandCheckList.push_back(&IsFourTwosThreesFoursWithAce);
	HandCheckList.push_back(&IsFourAces);
	HandCheckList.push_back(&IsStraightFlush);
	HandCheckList.push_back(&IsFourTwosThreesFours);
	HandCheckList.push_back(&IsFourFivesThruKings);
	HandCheckList.push_back(&IsFullHouse);
	HandCheckList.push_back(&IsFlush);
	HandCheckList.push_back(&IsStraight);
	HandCheckList.push_back(&IsThreeOfAKind);
	HandCheckList.push_back(&IsTwoPair);
	HandCheckList.push_back(&IsJacksOrBetter);
	HandCheckList.push_back(&IsNothing);
	/**/

	//  HAND POTENTIAL CHECKS
	HandPotentialCheckList.push_back(std::pair<HandPotentialCheck, char>(&CheckHand, Move(true, true, true, true, true)));
	HandPotentialCheckList.push_back(std::pair<HandPotentialCheck, char>(&CheckPotentialXHHHH, Move(false, true, true, true, true)));
	HandPotentialCheckList.push_back(std::pair<HandPotentialCheck, char>(&CheckPotentialHXHHH, Move(true, false, true, true, true)));
	HandPotentialCheckList.push_back(std::pair<HandPotentialCheck, char>(&CheckPotentialHHXHH, Move(true, true, false, true, true)));
	HandPotentialCheckList.push_back(std::pair<HandPotentialCheck, char>(&CheckPotentialHHHXH, Move(true, true, true, false, true)));
	HandPotentialCheckList.push_back(std::pair<HandPotentialCheck, char>(&CheckPotentialHHHHX, Move(true, true, true, true, false)));
	HandPotentialCheckList.push_back(std::pair<HandPotentialCheck, char>(&CheckPotentialXXHHH, Move(false, false, true, true, true)));
	HandPotentialCheckList.push_back(std::pair<HandPotentialCheck, char>(&CheckPotentialXHXHH, Move(false, true, false, true, true)));
	HandPotentialCheckList.push_back(std::pair<HandPotentialCheck, char>(&CheckPotentialXHHXH, Move(false, true, true, false, true)));
	HandPotentialCheckList.push_back(std::pair<HandPotentialCheck, char>(&CheckPotentialXHHHX, Move(false, true, true, true, false)));
	HandPotentialCheckList.push_back(std::pair<HandPotentialCheck, char>(&CheckPotentialHXXHH, Move(true, false, false, true, true)));
	HandPotentialCheckList.push_back(std::pair<HandPotentialCheck, char>(&CheckPotentialHXHXH, Move(true, false, true, false, true)));
	HandPotentialCheckList.push_back(std::pair<HandPotentialCheck, char>(&CheckPotentialHXHHX, Move(true, false, true, true, false)));
	HandPotentialCheckList.push_back(std::pair<HandPotentialCheck, char>(&CheckPotentialHHXXH, Move(true, true, false, false, true)));
	HandPotentialCheckList.push_back(std::pair<HandPotentialCheck, char>(&CheckPotentialHHXHX, Move(true, true, false, true, false)));
	HandPotentialCheckList.push_back(std::pair<HandPotentialCheck, char>(&CheckPotentialHHHXX, Move(true, true, true, false, false)));
	HandPotentialCheckList.push_back(std::pair<HandPotentialCheck, char>(&CheckPotentialXXXHH, Move(false, false, false, true, true)));
	HandPotentialCheckList.push_back(std::pair<HandPotentialCheck, char>(&CheckPotentialXXHXH, Move(false, false, true, false, true)));
	HandPotentialCheckList.push_back(std::pair<HandPotentialCheck, char>(&CheckPotentialXXHHX, Move(false, false, true, true, false)));
	HandPotentialCheckList.push_back(std::pair<HandPotentialCheck, char>(&CheckPotentialXHXXH, Move(false, true, false, false, true)));
	HandPotentialCheckList.push_back(std::pair<HandPotentialCheck, char>(&CheckPotentialXHXHX, Move(false, true, false, true, false)));
	HandPotentialCheckList.push_back(std::pair<HandPotentialCheck, char>(&CheckPotentialXHHXX, Move(false, true, true, false, false)));
	HandPotentialCheckList.push_back(std::pair<HandPotentialCheck, char>(&CheckPotentialHXXXH, Move(true, false, false, false, true)));
	HandPotentialCheckList.push_back(std::pair<HandPotentialCheck, char>(&CheckPotentialHXXHX, Move(true, false, false, true, false)));
	HandPotentialCheckList.push_back(std::pair<HandPotentialCheck, char>(&CheckPotentialHXHXX, Move(true, false, true, false, false)));
	HandPotentialCheckList.push_back(std::pair<HandPotentialCheck, char>(&CheckPotentialHHXXX, Move(true, true, false, false, false)));
	HandPotentialCheckList.push_back(std::pair<HandPotentialCheck, char>(&CheckPotentialXXXXH, Move(false, false, false, false, true)));
	HandPotentialCheckList.push_back(std::pair<HandPotentialCheck, char>(&CheckPotentialXXXHX, Move(false, false, false, true, false)));
	HandPotentialCheckList.push_back(std::pair<HandPotentialCheck, char>(&CheckPotentialXXHXX, Move(false, false, true, false, false)));
	HandPotentialCheckList.push_back(std::pair<HandPotentialCheck, char>(&CheckPotentialXHXXX, Move(false, true, false, false, false)));
	HandPotentialCheckList.push_back(std::pair<HandPotentialCheck, char>(&CheckPotentialHXXXX, Move(true, false, false, false, false)));
	HandPotentialCheckList.push_back(std::pair<HandPotentialCheck, char>(&CheckPotentialXXXXX, Move(false, false, false, false, false)));
}

char DetermineBestMove(River& river, double& averagePay)
{
	char returnMove = 0;

	double returnValue = 0;
	for (HandPotentialCheckListType::iterator iter = HandPotentialCheckList.begin(); iter != HandPotentialCheckList.end(); ++iter)
	{
		double value = (*iter).first(river.Card1, river.Card2, river.Card3, river.Card4, river.Card5);
		if (value > returnValue)
		{
			returnValue = value;
			returnMove = (*iter).second;
		}
	}

	averagePay = returnValue;
	return returnMove;
}

void OutputStatistics()
{
	unsigned int totalInput = 0;
	unsigned int totalReturn = 0;
	for (int i = 0; i < HAND_TYPE_COUNT; ++i)
	{
		std::cout << WinningHands[i].first << " - " << WinningHands[i].second << std::endl;
		totalReturn += (WinningHands[i].second * WinningValue[i]);
	}
	
	totalInput = MAX_RIVER_LAYOUTS * 5;

	std::cout << std::endl << std::endl;
	std::cout << "Hand Count:     " << MAX_RIVER_LAYOUTS << std::endl;
	std::cout << "Total Input:    (" << totalInput << ")" << std::endl;
	std::cout << "Total Return:   (" << totalReturn << ")" << std::endl;
	std::cout << "Return Percent: (" << double(totalReturn) / double(totalInput) * 100.0 << "%)" << std::endl;
	std::cout << "SIMULATION COMPLETE" << std::endl;
}