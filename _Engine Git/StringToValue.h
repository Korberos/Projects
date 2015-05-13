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
/*  File:       StringToValue.h                                  */
/*                                                               */
/*  Purpose:    This file contains basic functionality to grab   */
/*              values of different data types from a string.    */
/*                                                               */
/*  Created:    12/07/2008                                       */
/*  Last Edit:  01/16/2011                                       */
/*****************************************************************/

enum CanBeValueBitFlags
{
	STRING_CAN_BE_SIGNED_CHAR = 1,
	STRING_CAN_BE_SIGNED_SHORT = 2,
	STRING_CAN_BE_SIGNED_INT = 4,
	STRING_CAN_BE_SIGNED_LONG_INT = 8,
	STRING_CAN_BE_UNSIGNED_CHAR = 16,
	STRING_CAN_BE_UNSIGNED_SHORT = 32,
	STRING_CAN_BE_UNSIGNED_INT = 64,
	STRING_CAN_BE_UNSIGNED_LONG_INT = 128,
	STRING_CAN_BE_FLOAT = 256,
	STRING_CAN_BE_DOUBLE = 512,
	STRING_CAN_BE_ALL = 1023
};

typedef int StringCanBeBitflag;
StringCanBeBitflag StringCanBe( const char* string );

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

bool StringToSignedChar( const char* string, signed char& signed_char_val );
bool StringToSignedShort( const char* string, signed short& signed_short_val );
bool StringToSignedInt( const char* string, signed int& signed_int_val );
bool StringToSignedLongInt( const char* string, signed long int& signed_long_int_val );
bool StringToUnsignedChar( const char* string, unsigned char& unsigned_char_val );
bool StringToUnsignedShort( const char* string, unsigned short& unsigned_short_val );
bool StringToUnsignedInt( const char* string, unsigned int& unsigned_int_val );
bool StringToUnsignedLongInt( const char* string, unsigned long int& unsigned_long_int_val );
bool StringToFloat( const char* string, float& float_val );
bool StringToDouble( const char* string, double& double_val );