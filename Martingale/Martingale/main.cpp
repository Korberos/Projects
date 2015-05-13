
#include <iostream>
#include <ctime>

const int StartingMoney = 155;
const int GoalWinnings = 50;
const int MinimumBet = 5;
const int MaximumBet = 20000;
const int SimulationCount = 1000000;

int ContiguousFailure = 0;
int HighestContiguousFailure = 0;

bool RouletteSpin( void )
{
	//  There are 38 numbers on the roulette wheel. 18 Red, 18 Black, and 2 Green. A single bet on either red
	//  or black means you have an 18 / 38 chance to succeed.
	bool result = ( ( rand() % 38 ) < 18 );
	if ( result == true )
	{
		ContiguousFailure = 0;
	}
	else
	{
		ContiguousFailure += 1;
		if ( ContiguousFailure > HighestContiguousFailure )
		{
			HighestContiguousFailure = ContiguousFailure;
		}
	}

	return result;
}


bool DoubleYourMoney( int minimum_bet )
{
	HighestContiguousFailure = 0;

	int money = StartingMoney;
	if ( money >= StartingMoney + GoalWinnings )
	{
		return true;
	}

	for ( int bet = minimum_bet; money >= minimum_bet;  bet *= 2 )
	{
		money -= bet;

		if ( RouletteSpin() )
		{
			money += bet * 2;
			if ( money >= StartingMoney + GoalWinnings )
			{
				return true;
			}
			bet = minimum_bet / 2;
			continue;
		}
	}

	return false;
}


int PlayUntilContiguousLoss( const int base_bet, const int max_contiguous_losses )
{
	int return_cash = 0;
	int contiguous_losses = 0;

	while ( contiguous_losses < max_contiguous_losses )
	{
		bool success = RouletteSpin();
		if ( !success )
		{
			++contiguous_losses;
			continue;
		}
		else
		{
			return_cash += base_bet;
			contiguous_losses = 0;
		}
	}

	return return_cash;
}


float Calculate_Success_Ratio( int starting_value )
{
	unsigned int success_count = 0;
	for ( unsigned int i = 0; i < SimulationCount; ++i )
	{
		success_count += ( DoubleYourMoney( starting_value ) ? 1 : 0 );
	}

	float success_ratio = 100.0f * float(success_count) / float(SimulationCount);
	std::cout << starting_value << "\t|\t" << success_ratio << "\t|\t" << HighestContiguousFailure << std::endl;

	return success_ratio;
}


void main( void )
{
	//  Seed the random number generator
	srand( (unsigned int)(time( 0 )) );

	//  Determine the max contiguous losses
	int max_contiguous = 0;
	int sum_of_bets = 0;
	while ( true )
	{
		int sum = MinimumBet;
		for ( int i = 0; i < max_contiguous + 1; ++i ) sum *= 2;
		if ( sum - MinimumBet > StartingMoney ) break;
		else
		{
			++max_contiguous;
			sum_of_bets = sum - MinimumBet;
		}
	}

	std::cout << "Betting $" << MinimumBet << std::endl;
	std::cout << "Bringing $" << StartingMoney << std::endl;
	std::cout << "Max Losses " << max_contiguous << std::endl;
	std::cout << "Sum of Losses $" << sum_of_bets << std::endl;

	long int final_money = 0;
	for ( int i = 0; i < SimulationCount; ++i )
	{
		final_money += (long int)(PlayUntilContiguousLoss( MinimumBet, max_contiguous ));
		final_money -= sum_of_bets;
	}
	float average_win = float(final_money) / float(SimulationCount);
	std::cout << "Average Win in PUCL = " << average_win << " per day." << std::endl;
}


//void main( void )
//{
//	//  Seed the random number generator
//	srand( (unsigned int)(time( 0 )) );
//
//	std::cout << "Minimum Bet | Success Ratio | Max Contiguous Failures" << std::endl;
//
//	float highest_success_ratio = 0.0f;
//	int highest_success_index = -1;
//	for ( unsigned int i = MinimumBet; i <= GoalWinnings; ++i )
//	{
//		float success_ratio = Calculate_Success_Ratio( i );
//		if ( success_ratio > highest_success_ratio )
//		{
//			highest_success_ratio = success_ratio;
//			highest_success_index = i;
//		}
//	}
//
//	std::cout << std::endl;
//	std::cout << "Attempting to make $" << GoalWinnings << " in winnings from $" << StartingMoney << " starting credit..." << std::endl;
//	std::cout << "Highest success ratio:" << std::endl;
//	std::cout << "Minimum bet $" << highest_success_index << " - Success rate: " << highest_success_ratio << "%" << std::endl;
//}