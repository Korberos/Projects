#include <iostream>
#include "StringToValue.h"

char test_number[] = "18446744073709551616";

void main(void)
{
	short test;
	std::cout << StringToSignedShort(test_number, test) << std::endl;
}