#include <iostream>
#include <vector>
#include <ctime>
#include <limits>
#include <string>

#define CREDITS_TO_ADD 1000

const char NUMBERS[13] = { '2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K', 'A' };
const char SUITS[4] = { 'C', 'D', 'H', 'S' };


void Output_Card( int card )
{
	//  The card is a number from 0 to 51, and the arrays are 0-12 and 0-3, allowing
	//  you to easily gather the numbers using simple math. An additional plus is that
	//  since the number value needs to be divided by 4, we can use bit shifting instead
	//  of a divide call. As for the suit, we are just checking the last 3 bits to
	//  determine the modulus, so we can use bit math for that as well. Instead of 
	//  calling "card % 4", we can use "card & 3"
	std::cout << NUMBERS[ card >> 2 ] << SUITS[ card & 3 ];
}


class Game
{
public:
	Game();
	~Game();

private:
	void Main_Loop();
	void Add_Credits();
	void Show_Credits();
	void Show_Help();
	void Play();

	unsigned int Credits;
	unsigned int Bet;
};

Game::Game() : 
	Credits( 0 ),
	Bet( 0 )
{
	Show_Credits();
	Main_Loop();
}


Game::~Game()
{
	//  Do nothing, since there is no dynamic memory.
}

void Game::Main_Loop()
{
	while ( true )
	{
		// Show the main menu options
		std::cout << "(H)elp, (A)dd Credit, (Q)uit";
		if ( Credits > 0 )
		{
			std::cout << " or enter bet [" << Bet << "]";
		}
		std::cout << ": ";

		//  Take in the player input for their choice of action
		std::string choice_string;
		getline( std::cin, choice_string );

		//  Take action based on the input value
		switch ( choice_string[0] )
		{
		case 'H':
		case 'h':
			//  Show the help menu, then return to the loop again to show the main menu
			Show_Help();
			break;

		case 'A':
		case 'a':
			//  Add a certain amount of credits to the count, then return to the loop again to show the main menu
			Add_Credits();
			break;

		case 'Q':
		case 'q':
			//  Say goodbye to the player, then return out, ending the game
			std::cout << "Thanks for Playing!" << std::endl;
			return;

		default:
			//  If we have no credits and the main three options weren't chosen, we know the input is invalid.
			if ( Credits < 1 )
			{
				std::cout << "Invalid option. Please try again." << std::endl;
				break;
			}

			//  If the player just hits enter with no value, assume the last bet played
			if ( choice_string.length() == 0 )
			{
				Play();
				break;
			}

			//  Grab the bet value from the string
			int bet = atoi( choice_string.c_str() );

			//  If the bet is less than 1, the input is invalid, but it can be for multiple reasons
			if ( bet < 1 )
			{
				if ( bet == 0 )
				{
					bool valid_number = true;
					for ( unsigned int i = 0; i < choice_string.length(); ++i )
					{
						if ( choice_string[i] < '0' || choice_string[i] > '9' )
						{
							valid_number = false;
							break;
						}
					}

					if ( valid_number == false )
					{
						std::cout << "Invalid option. Please try again." << std::endl;
						break;
					}

					std::cout << "You can not bet 0 credits. Please try again." << std::endl;
					break;
				}
				else
				{
					std::cout << "You can not bet a number below 0!. Please try again." << std::endl;
					break;
				}
			}
			else if ( (unsigned int)(bet) > Credits )
			{
				std::cout << "You do not have " << bet << " credits. You only have " << Credits << " credits. Please try again." << std::endl;
				break;
			}
			else
			{
				Bet = bet;
				Play();
				break;
			}
			return;
		}
	}
}


void Game::Add_Credits()
{
	//  Add the base amount of credits to the count, and then increment the bet if the bet was 0
	Credits += CREDITS_TO_ADD;
	Bet = std::max<int>( Bet, 1 );

	std::cout << "Add Credit: " << CREDITS_TO_ADD << std::endl;
	Show_Credits();
}


void Game::Show_Credits()
{
	//  Show the amount of credits
	std::cout << "Credit=" << Credits << std::endl;
}


void Game::Show_Help()
{
	//  Show the help dialog (copied exactly from the instructions, down to the line breaks, just in case)
	std::cout << std::endl;
	std::cout << "*** Beat the Dealer ***" << std::endl;
	std::cout << "Enter the amount you wish to bet." << std::endl;
	std::cout << "5 cards will be dealt." << std::endl;
	std::cout << "The first card is the dealer card and is dealt face-up." << std::endl;
	std::cout << "The remaining 4 cards will be dealt face-down." << std::endl;
	std::cout << "Enter the position of one of the face-down cards." << std::endl;
	std::cout << "If your card is higher than the dealer card, you win twice your" << std::endl;
	std::cout << "bet." << std::endl;
	std::cout << "If your card is of the same value, you win your bet back." << std::endl;
	std::cout << "Aces are high!" << std::endl;
	std::cout << "Good Luck!" << std::endl << std::endl;
}


void Game::Play()
{
	//  Remove the betting amount from the credits
	Credits -= Bet;

	//  grab a random card for the dealer
	int dealer_card = rand() % 52;

	// There are two different mind-sets to this problem. One is that the player's cards
	// should be drawn out into memory because in theory, it makes the game an even playing
	// field (in theory, not in practice). The other is that since there is no need to put
	// the player's cards into memory, we can just select a random card when the player makes
	// a choice, since that is when we need the information. For this example, I will draw
	// the cards into memory first, if only for example's sake.

	//  Draw 4 random player cards, ensuring they are not the same as the dealers or each others
	int player_card[4] = { -1, -1, -1, -1 };
	for ( unsigned int i = 0; i < 4; ++i )
	{
		while ( true )
		{
			player_card[i] = rand() % 52;

			//  If the card is the same as the dealer, try again
			if ( dealer_card == player_card[i] ) { continue; }

			//  If the card is the same as any other card, try again
			bool card_in_use = false;
			for ( unsigned int j = 0; j < 4; ++j )
			{
				if ( i == j ) continue;
				card_in_use |= player_card[i] == player_card[j];
			}
			if ( card_in_use == true ) { continue; }

			break;
		}
	}

	//  Show the cards
	std::cout << "-------------------------------------------------------------" << std::endl;
	Show_Credits();
	std::cout << "Dealer: ";
	Output_Card( dealer_card );
	std::cout << ", Player: ** ** ** **" << std::endl;

	//  Allow the player to select a card
	while ( true )
	{
		std::cout << "Select: ";
		std::string selection_string;
		getline( std::cin, selection_string );

		//  If the player did not select a card, tell the player and try again
		if ( selection_string.length() == 0 )
		{
			std::cout << "You must select a number." << std::endl;
			continue;
		}

		//  If the selection is not between 1 and 4, tell the player and try again
		int selection = atoi( selection_string.c_str() );
		if ( selection < 1 || selection > 4 )
		{
			std::cout << "Please enter a number from 1 to 4" << std::endl;
			continue;
		}
		else
		{
			//  set select to a number between 0 and 3 instead of 1 and 4, for array index purposes
			selection -= 1;

			//  Show the cards and the player's choice
			std::cout << "Dealer: ";
			Output_Card( dealer_card );
			std::cout << ", Player: ";
			for ( unsigned int i = 0; i < 4; ++i )
			{
				//  Highlight the chosen card, just display the others
				if ( i == selection )
				{
					std::cout << "<";
					Output_Card( player_card[i] );
					std::cout << "> ";
				}
				else
				{
					Output_Card( player_card[i] );
					std::cout << " ";
				}
			}
			std::cout << std::endl;
		}

		if ( dealer_card >> 2 > player_card[selection] >> 2 )
		{
			//  If your number card is below the dealer, you lose
			std::cout << "Dealer Wins!" << std::endl;
			break;
		}
		else if ( dealer_card >> 2 > player_card[selection] >> 2 )
		{
			//  If your number card is even to the dealer, you get your bet back
			std::cout << "Push! Win=" << Bet;
			Credits += Bet;
			break;
		}
		else
		{
			//  If your number card is above the dealer, you win double your bet
			std::cout << "Player Wins! Win=" << ( Bet << 1 ) << std::endl;
			Credits += Bet << 1;
			break;
		}
	}

	//  Show the updated amount of credits
	Show_Credits();
}


void main()
{
	//  Seed a random number generator
	srand( (unsigned int)(time( 0 ) ) );

	//  Just create the game class. It will automatically handle the game.
	Game game;
}