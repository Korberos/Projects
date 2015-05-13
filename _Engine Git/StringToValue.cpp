/*****************************************************************/
/*	           ___                          _  _                  */
/*	    	    / _ \                        | |(_)                 */
/*          / /_\ \ _ __   ___   __ _   __| | _   __ _           */
/*    	   |  _  || '__| / __| / _` | / _` || | / _` |          */
/*	         | | | || |   | (__ | (_| || (_| || || (_| |          */
/*	         \_| |_/|_|    \___| \__,_| \__,_||_| \__,_|          */
/*                                                               */
/*                                      Engine Version 01.00.00  */
/*****************************************************************/
/*  File:       StringToValue.cpp                                */
/*                                                               */
/*  Purpose:    This file contains basic functionality to grab   */
/*              values of different data types from a string.    */
/*                                                               */
/*  Created:    12/07/2008                                       */
/*  Last Edit:  04/18/2009                                       */
/*****************************************************************/

typedef int StringCanBeBitflag;

#include "StringToValue.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


/*
//  For reference - Not used in code
#define SIGNED_CHAR_MIN			-128;
#define SIGNED_CHAR_MAX			127;
#define SIGNED_SHORT_MIN		-32768
#define SIGNED_SHORT_MAX		32767
#define SIGNED_INT_MIN			-2147483648
#define SIGNED_INT_MAX			2147483647
#define SIGNED_LONG_INT_MIN		-9223372036854775808
#define SIGNED_LONG_INT_MAX 	9223372036854775807
#define UNSIGNED_CHAR_MIN		0
#define UNSIGNED_CHAR_MAX		255
#define UNSIGNED_SHORT_MIN		0
#define UNSIGNED_SHORT_MAX		65535
#define UNSIGNED_INT_MIN		0
#define UNSIGNED_INT_MAX		4294967295
#define UNSIGNED_LONG_INT_MIN	0
#define UNSIGNED_LONG_INT_MAX	18446744073709551615
#define FLOAT_MIN				
#define FLOAT_MAX				
#define DOUBLE_MIN				
#define DOUBLE_MAX	
*/

void StringCanBeSignedChar( const char* string, StringCanBeBitflag& flag );
void StringCanBeSignedShort( const char* string, StringCanBeBitflag& flag );
void StringCanBeSignedInt( const char* string, StringCanBeBitflag& flag );
void StringCanBeSignedLongInt( const char* string, StringCanBeBitflag& flag );
void StringCanBeUnsignedChar( const char* string, StringCanBeBitflag& flag );
void StringCanBeUnsignedShort( const char* string, StringCanBeBitflag& flag );
void StringCanBeUnsignedInt( const char* string, StringCanBeBitflag& flag );
void StringCanBeUnsignedLongInt( const char* string, StringCanBeBitflag& flag );
void StringCanBeFloat( const char* string, StringCanBeBitflag& flag );
void StringCanBeDouble( const char* string, StringCanBeBitflag& flag );

StringCanBeBitflag StringCanBe( const char* string )
{
	StringCanBeBitflag flag = STRING_CAN_BE_ALL;

	bool Decimal = false;
	for (int i = 0; string[i] != 0; i++)
	{
		//  If there is a - symbol, check if it is at the beginning. If it is, disregard unsigned numbers. It it isn't, disregard all numbers.
		if (string[i] == '-')
		{
			if (i == 0)
			{
				flag &= ~STRING_CAN_BE_UNSIGNED_CHAR;
				flag &= ~STRING_CAN_BE_UNSIGNED_SHORT;
				flag &= ~STRING_CAN_BE_UNSIGNED_INT;
				flag &= ~STRING_CAN_BE_UNSIGNED_LONG_INT;
				continue;
			}
			else
			{
				//  There is a '-' symbol somewhere other than the beginning... it can't be a number
				return 0;
			}
		}

		if (string[i] == 'f')
		{
			if ((flag & STRING_CAN_BE_FLOAT) == 0 || string[i+1] != 0)
			{
				//  If it contains an 'f' but isn't a float, it can't be a number
				return 0;
			}

			//  There is an 'f' at the end. It can only be a float
			return STRING_CAN_BE_FLOAT;
		}

		//  If there is more than one decimal, the string can not be a float or double
		if (string[i] == '.')
		{
			flag &= ~STRING_CAN_BE_SIGNED_CHAR;
			flag &= ~STRING_CAN_BE_SIGNED_SHORT;
			flag &= ~STRING_CAN_BE_SIGNED_INT;
			flag &= ~STRING_CAN_BE_SIGNED_LONG_INT;
			flag &= ~STRING_CAN_BE_UNSIGNED_CHAR;
			flag &= ~STRING_CAN_BE_UNSIGNED_SHORT;
			flag &= ~STRING_CAN_BE_UNSIGNED_INT;
			flag &= ~STRING_CAN_BE_UNSIGNED_LONG_INT;

			if (Decimal == true || i == 0 || string[i+1] == 0)
			{
				//  There is a second decimal, or a decimal in an improper place... it can't be a number
				return 0;
			}

			Decimal = true;
			continue;
		}

		//  If any character is not a number, the string can not be a char, short, int, or long int
		if (string[i] < '0' || string[i] > '9')
		{
			flag &= ~STRING_CAN_BE_SIGNED_CHAR;
			flag &= ~STRING_CAN_BE_SIGNED_SHORT;
			flag &= ~STRING_CAN_BE_SIGNED_INT;
			flag &= ~STRING_CAN_BE_SIGNED_LONG_INT;
			flag &= ~STRING_CAN_BE_UNSIGNED_CHAR;
			flag &= ~STRING_CAN_BE_UNSIGNED_SHORT;
			flag &= ~STRING_CAN_BE_UNSIGNED_INT;
			flag &= ~STRING_CAN_BE_UNSIGNED_LONG_INT;
		}
	}

	if (flag & STRING_CAN_BE_SIGNED_CHAR)			StringCanBeSignedChar( string, flag );
	if (flag & STRING_CAN_BE_SIGNED_SHORT)			StringCanBeSignedShort( string, flag );
	if (flag & STRING_CAN_BE_SIGNED_INT)			StringCanBeSignedInt( string, flag );
	if (flag & STRING_CAN_BE_SIGNED_LONG_INT)		StringCanBeSignedLongInt( string, flag );
	if (flag & STRING_CAN_BE_UNSIGNED_CHAR)		StringCanBeUnsignedChar( string, flag );
	if (flag & STRING_CAN_BE_UNSIGNED_SHORT)		StringCanBeUnsignedShort( string, flag );
	if (flag & STRING_CAN_BE_UNSIGNED_INT)			StringCanBeUnsignedInt( string, flag );
	if (flag & STRING_CAN_BE_UNSIGNED_LONG_INT)	StringCanBeUnsignedLongInt( string, flag );
	if (flag & STRING_CAN_BE_FLOAT)					StringCanBeFloat( string, flag );
	if (flag & STRING_CAN_BE_DOUBLE)					StringCanBeDouble( string, flag );

	return flag;
}

void StringCanBeSignedChar( const char* string, StringCanBeBitflag& flag )
{
	bool negative = (string[0] == '-');

	int length;
	for (length = 0; string[length] != 0; length++) {}

	//  If the length of the string indicates that it is between 99 and -99, it can be a signed char
	if (length < (negative ? 4 : 3))
	{
		return;
	}

	//  If the length of the string indicates that it is either > 999 or < -999, it can not be a signed char
	if (length > (negative ? 4 : 3))
	{
		flag &= ~STRING_CAN_BE_SIGNED_CHAR;
		return;
	}

	//  If the number is < -128 or > 127, it can not be a signed char
	const char* positive_string = negative ? &string[1] : string;
	if (positive_string[0] > '1'
	 || positive_string[1] > '2'
	 || positive_string[2] > (negative ? '8' : '7'))
	{
		flag &= ~STRING_CAN_BE_SIGNED_CHAR;
		return;
	}
}

void StringCanBeSignedShort( const char* string, StringCanBeBitflag& flag )
{
	bool negative = (string[0] == '-');

	int length;
	for (length = 0; string[length] != 0; length++) {}

	//  If the length of the string indicates that it is between 9999 and -9999, it can be a signed short
	if (length < (negative ? 6 : 5))
	{
		return;
	}

	//  If the length of the string indicates that it is either > 99999 or < -99999, it can not be a signed short
	if (length > (negative ? 6 : 5))
	{
		flag &= ~STRING_CAN_BE_SIGNED_SHORT;
		return;
	}

	//  If the number is < -32768 or > 32767, it can not be a signed char
	const char* positive_string = negative ? &string[1] : string;
	if (positive_string[0] > '3'
	 || positive_string[1] > '2'
	 || positive_string[2] > '7'
	 || positive_string[3] > '6'
	 || positive_string[4] > (negative ? '8' : '7'))
	{
		flag &= ~STRING_CAN_BE_SIGNED_SHORT;
		return;
	}
}

void StringCanBeSignedInt( const char* string, StringCanBeBitflag& flag )
{
	bool negative = (string[0] == '-');

	int length;
	for (length = 0; string[length] != 0; length++) {}

	//  If the length of the string indicates that it is between 99 and -99, it can be a signed int
	if (length < (negative ? 11 : 10))
	{
		return;
	}

	//  If the length of the string indicates that it is either > 999 or < -999, it can not be a signed int
	if (length > (negative ? 11 : 10))
	{
		flag &= ~STRING_CAN_BE_SIGNED_INT;
		return;
	}

	//  If the number is < -2147483648 or > 2147483647, it can not be a signed char
	const char* positive_string = negative ? &string[1] : string;
	if (positive_string[0] > '2'
	 || positive_string[1] > '1'
	 || positive_string[2] > '4'
	 || positive_string[3] > '7'
	 || positive_string[4] > '4'
	 || positive_string[5] > '8'
	 || positive_string[6] > '3'
	 || positive_string[7] > '6'
	 || positive_string[8] > '4'
	 || positive_string[9] > (negative ? '8' : '7'))
	{
		flag &= ~STRING_CAN_BE_SIGNED_INT;
		return;
	}
}

void StringCanBeSignedLongInt( const char* string, StringCanBeBitflag& flag )
{
	bool negative = (string[0] == '-');

	int length;
	for (length = 0; string[length] != 0; length++) {}

	//  If the length of the string indicates that it is between 9999999999999999999 and -9999999999999999999, it can be a signed long int
	if (length < (negative ? 20 : 19))
	{
		return;
	}

	//  If the length of the string indicates that it is either > 99999999999999999999 or < -99999999999999999999, it can not be a signed long int
	if (length > (negative ? 20 : 19))
	{
		flag &= ~STRING_CAN_BE_SIGNED_LONG_INT;
		return;
	}

	//  If the number is < -9223372036854775808 or > 9223372036854775807, it can not be a signed long int
	const char* positive_string = negative ? &string[1] : string;
	if (positive_string[0] > '9'
	 || positive_string[1] > '2'
	 || positive_string[2] > '2'
	 || positive_string[3] > '3'
	 || positive_string[4] > '3'
	 || positive_string[5] > '7'
	 || positive_string[6] > '2'
	 || positive_string[7] > '0'
	 || positive_string[8] > '3'
	 || positive_string[9] > '6'
	 || positive_string[10] > '8'
	 || positive_string[11] > '5'
	 || positive_string[12] > '4'
	 || positive_string[13] > '7'
	 || positive_string[14] > '7'
	 || positive_string[15] > '5'
	 || positive_string[16] > '8'
	 || positive_string[17] > '0'
	 || positive_string[18] > (negative ? '8' : '7'))
	{
		flag &= ~STRING_CAN_BE_SIGNED_LONG_INT;
		return;
	}
}

void StringCanBeUnsignedChar( const char* string, StringCanBeBitflag& flag )
{
	int length;
	for (length = 0; string[length] != 0; length++) {}

	//  If the length of the string indicates that it is < 99, it can be an unsigned char
	if (length < 3)
	{
		return;
	}

	//  If the length of the string indicates that it is > 999, it can not be an unsigned char
	if (length > 3)
	{
		flag &= ~STRING_CAN_BE_UNSIGNED_CHAR;
		return;
	}

	//  If the number is > 255, it can not be an unsigned char
	if (string[0] > '2'
	 || string[1] > '5'
	 || string[2] > '5')
	{
		flag &= ~STRING_CAN_BE_UNSIGNED_CHAR;
		return;
	}
}

void StringCanBeUnsignedShort( const char* string, StringCanBeBitflag& flag )
{
	int length;
	for (length = 0; string[length] != 0; length++) {}

	//  If the length of the string indicates that it is < 9999, it can be an unsigned short
	if (length < 5)
	{
		return;
	}

	//  If the length of the string indicates that it is > 99999, it can not be an unsigned short
	if (length > 5)
	{
		flag &= ~STRING_CAN_BE_UNSIGNED_SHORT;
		return;
	}

	//  If the number is > 65535, it can not be an unsigned short
	if (string[0] > '6'
	 || string[1] > '5'
	 || string[2] > '5'
	 || string[3] > '3'
	 || string[4] > '5')
	{
		flag &= ~STRING_CAN_BE_UNSIGNED_SHORT;
		return;
	}
}

void StringCanBeUnsignedInt( const char* string, StringCanBeBitflag& flag )
{
	int length;
	for (length = 0; string[length] != 0; length++) {}

	//  If the length of the string indicates that it is < 999999999, it can be an unsigned int
	if (length < 10)
	{
		return;
	}

	//  If the length of the string indicates that it is > 9999999999, it can not be an unsigned int
	if (length > 10)
	{
		flag &= ~STRING_CAN_BE_UNSIGNED_INT;
		return;
	}

	//  If the number is > 4294967295, it can not be an unsigned int
	if (string[0] > '4'
	 || string[1] > '2'
	 || string[2] > '9'
	 || string[3] > '4'
	 || string[4] > '9'
	 || string[5] > '6'
	 || string[6] > '7'
	 || string[7] > '2'
	 || string[8] > '9'
	 || string[9] > '5')
	{
		flag &= ~STRING_CAN_BE_UNSIGNED_INT;
		return;
	}
}

void StringCanBeUnsignedLongInt( const char* string, StringCanBeBitflag& flag )
{
	int length;
	for (length = 0; string[length] != 0; length++) {}

	//  If the length of the string indicates that it is < 9999999999999999999, it can be an unsigned long int
	if (length < 20)
	{
		return;
	}

	//  If the length of the string indicates that it is > 99999999999999999999, it can not be an unsigned long int
	if (length > 20)
	{
		flag &= ~STRING_CAN_BE_UNSIGNED_LONG_INT;
		return;
	}

	//  If the number is > 18446744073709551615, it can not be an unsigned long int
	if (string[0] > '1'
	 || string[1] > '8'
	 || string[2] > '4'
	 || string[3] > '4'
	 || string[4] > '6'
	 || string[5] > '7'
	 || string[6] > '4'
	 || string[7] > '4'
	 || string[8] > '0'
	 || string[9] > '7'
	 || string[10] > '3'
	 || string[11] > '7'
	 || string[12] > '0'
	 || string[13] > '9'
	 || string[14] > '5'
	 || string[15] > '5'
	 || string[16] > '1'
	 || string[17] > '6'
	 || string[18] > '1'
	 || string[19] > '5')
	{
		flag &= ~STRING_CAN_BE_UNSIGNED_LONG_INT;
		return;
	}
}

void StringCanBeFloat( const char* string, StringCanBeBitflag& flag )
{
	int length;
	for (length = 0; string[length] != 0; ++length) {}

	float value = float(atof(string));

	if (value == HUGE_VAL)
	{
		flag &= ~STRING_CAN_BE_FLOAT;
		return;
	}

	const int zero_count = 6;
	if ( ( value > -1.0f ) && ( value < ( 1.0f / ( 10 * zero_count ) ) ) )
	{
		for (int i = 0; i < zero_count + 2 || i > length || string[i] == 0; i++)
		{
			if (string[i] != '0' && string[i] != '.')
			{
				flag &= ~STRING_CAN_BE_FLOAT;
				return;
			}
		}
	}
}

void StringCanBeDouble( const char* string, StringCanBeBitflag& flag )
{
	int length;
	for (length = 0; string[length] != 0; length++) {}

	double value = atof(string);
	if (value == HUGE_VAL)
	{
		flag &= ~STRING_CAN_BE_DOUBLE;
		return;
	}

	const int zero_count = 6;
	if (value < (1.0f / (10 * zero_count)))
	{
		for (int i = 0; i < zero_count + 2 || i > length || string[i] == 0; i++)
		{
			if (string[i] != '0' && string[i] != '.')
			{
				flag &= ~STRING_CAN_BE_DOUBLE;
				return;
			}
		}
	}
}

bool StringToSignedChar( const char* string, signed char& signed_char_val )
{
	if (!(StringCanBe(string) & STRING_CAN_BE_SIGNED_CHAR))
	{
		return false;
	}

	signed_char_val = (signed char)(atoi(string));
	return true;
}

bool StringToSignedShort( const char* string, signed short& signed_short_val )
{
	if (!(StringCanBe(string) & STRING_CAN_BE_SIGNED_SHORT))
	{
		return false;
	}

	signed_short_val = (signed short)(atoi(string));
	return true;
}

bool StringToSignedInt( const char* string, signed int& signed_int_val )
{
	if (!(StringCanBe(string) & STRING_CAN_BE_SIGNED_INT))
	{
		return false;
	}

	signed_int_val = (signed int)(atoi(string));
	return true;
}

bool StringToSignedLongInt( const char* string, signed long int& signed_long_int_val )
{
	if (!(StringCanBe(string) & STRING_CAN_BE_SIGNED_LONG_INT))
	{
		return false;
	}

	signed_long_int_val = (signed long int)(atoi(string));
	return true;
}

bool StringToUnsignedChar( const char* string, unsigned char& unsigned_char_val )
{
	if (!(StringCanBe(string) & STRING_CAN_BE_UNSIGNED_CHAR))
	{
		return false;
	}

	unsigned_char_val = (unsigned char)(atoi(string));
	return true;
}

bool StringToUnsignedShort( const char* string, unsigned short& unsigned_short_val )
{
	if (!(StringCanBe(string) & STRING_CAN_BE_UNSIGNED_SHORT))
	{
		return false;
	}

	unsigned_short_val = (unsigned short)(atoi(string));
	return true;
}

bool StringToUnsignedInt( const char* string, unsigned int& unsigned_int_val )
{
	if (!(StringCanBe(string) & STRING_CAN_BE_UNSIGNED_INT))
	{
		return false;
	}

	unsigned_int_val = (unsigned int)(atoi(string));
	return true;
}

bool StringToUnsignedLongInt( const char* string, unsigned long int& unsigned_long_int_val )
{
	if (!(StringCanBe(string) & STRING_CAN_BE_UNSIGNED_LONG_INT))
	{
		return false;
	}

	unsigned_long_int_val = (unsigned long int)(atoi(string));
	return true;
}

bool StringToFloat( const char* string, float& float_val )
{
	if (!(StringCanBe(string) & STRING_CAN_BE_FLOAT))
	{
		return false;
	}

	float_val = (float)(atof(string));
	return true;
}

bool StringToDouble( const char* string, double& double_val )
{
	if (!(StringCanBe(string) & STRING_CAN_BE_DOUBLE))
	{
		return false;
	}

	double_val = (double)(atof(string));
	return true;
}