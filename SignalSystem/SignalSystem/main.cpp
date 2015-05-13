#include "SignalSystem.h"

#include <iostream>

class TestGenerator : public SignalGenerator
{
public:
	TestGenerator() { test = 3; }

	void Test_Send( void )
	{
		SignalManager::Get_Instance()->Send_Signal( SignalType(2), this, (void*)(&test) );
	}
	
	int test;
};

class TestReceiver : public SignalReceiver
{
	void Receive_Signal( SignalType signal_type, void* signal_data )
	{
		switch ( signal_type )
		{
		case 1:
			{
				std::cout << "Signal type 1 received!" << std::endl;
			}
			break;

		case 2:
			{
				int* test_int = (int*)(signal_data);
				(*test_int)++;
				std::cout << "Signal type 2 received:  " << test_int[0] << std::endl;
			}
			break;
		}
	}
};

void main( void )
{
	TestGenerator test_generator;
	TestReceiver test_receiver;

	SignalManager::Get_Instance()->Listen_For_Signal( SignalType(1), &test_generator, &test_receiver );
	SignalManager::Get_Instance()->Listen_For_Signal( SignalType(2), &test_generator, &test_receiver );

	SignalManager::Get_Instance()->Send_Signal( SignalType(1), &test_generator, NULL );
	
	test_generator.Test_Send();

	SignalManager::Get_Instance()->Send_Global_Signal( SignalType(1), NULL );
}