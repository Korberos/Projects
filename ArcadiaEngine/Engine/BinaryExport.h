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
/*  File:       BinaryExport.h                                   */
/*                                                               */
/*  Purpose:    This file contains functionality to output a     */
/*              binary file from an XML document created to a    */
/*              certain specification.                           */
/*                                                               */
/*  Created:    01/02/2009                                       */
/*  Last Edit:  04/18/2010                                       */
/*****************************************************************/

#ifndef _BINARY_EXPORT_
#define _BINARY_EXPORT_

#include <fstream>
#include <vector>
#include "XML_File.h"
#include "StringToValue.h"

struct ExportElement
{
public:
	ExportElement()
	{
		Type = NULL;
		Value = NULL;
	}
	~ExportElement()
	{
		delete [] Type;
		Type = NULL;

		delete [] Value;
		Value = NULL;
	}

	char* Type;
	char* Value;
};

void ExportValue( std::ofstream& output_stream, char* type, char* value )
{
	if		( (strcmp( type, "char" ) == 0) && (StringCanBe( value ) & STRING_CAN_BE_SIGNED_CHAR ) )
	{
		signed char output_value;
		StringToSignedChar( value, output_value );
		output_stream.write((char*)&output_value, sizeof(output_value));
	}
	else if ( (strcmp( type, "short" ) == 0) && (StringCanBe( value ) & STRING_CAN_BE_SIGNED_SHORT ) )
	{
		signed short output_value;
		StringToSignedShort( value, output_value );
		output_stream.write((char*)&output_value, sizeof(output_value));
	}
	else if ( (strcmp( type, "int" ) == 0) && (StringCanBe( value ) & STRING_CAN_BE_SIGNED_INT ) )
	{
		signed int output_value;
		StringToSignedInt( value, output_value );
		output_stream.write((char*)&output_value, sizeof(output_value));
	}
	else if ( (strcmp( type, "long_int" ) == 0) && (StringCanBe( value ) & STRING_CAN_BE_SIGNED_LONG_INT ) )
	{
		signed long int output_value;
		StringToSignedLongInt( value, output_value );
		output_stream.write((char*)&output_value, sizeof(output_value));
	}
	else if ( (strcmp( type, "char" ) == 0) && (StringCanBe( value ) & STRING_CAN_BE_UNSIGNED_CHAR ) )
	{
		unsigned char output_value;
		StringToUnsignedChar( value, output_value );
		output_stream.write((char*)&output_value, sizeof(output_value));
	}
	else if ( (strcmp( type, "short" ) == 0) && (StringCanBe( value ) & STRING_CAN_BE_UNSIGNED_SHORT ) )
	{
		unsigned short output_value;
		StringToUnsignedShort( value, output_value );
		output_stream.write((char*)&output_value, sizeof(output_value));
	}
	else if ( (strcmp( type, "int" ) == 0) && (StringCanBe( value ) & STRING_CAN_BE_UNSIGNED_INT ) )
	{
		unsigned int output_value;
		StringToUnsignedInt( value, output_value );
		output_stream.write((char*)&output_value, sizeof(output_value));
	}
	else if ( (strcmp( type, "long_int" ) == 0) && (StringCanBe( value ) & STRING_CAN_BE_UNSIGNED_LONG_INT ) )
	{
		unsigned long int output_value;
		StringToUnsignedLongInt( value, output_value );
		output_stream.write((char*)&output_value, sizeof(output_value));
	}
	else if ( (strcmp( type, "float" ) == 0) && (StringCanBe( value ) & STRING_CAN_BE_FLOAT ) )
	{
		float output_value;
		StringToFloat( value, output_value );
		output_stream.write((char*)&output_value, sizeof(output_value));
	}
	else if ( (strcmp( type, "double" ) == 0) && (StringCanBe( value ) & STRING_CAN_BE_DOUBLE ) )
	{
		double output_value;
		StringToDouble( value, output_value );
		output_stream.write((char*)&output_value, sizeof(output_value));
	}
	else if (strcmp( type, "string" ) == 0)
	{
		output_stream.write((char*)value, strlen(value) + 1);
	}
}

bool BinaryExport( char* export_list_file )
{
	if ( export_list_file == 0 ) return false;

	XML_File* export_list_xml = XML_File::Load_XML_File( export_list_file );
	if ( export_list_xml == NULL ) return false;

	XML_Tag* entire_xml = export_list_xml->Get_Encompassing_Tag();
	if (entire_xml->Children.size() != 1) return false;

	//  Verify that the Binary Export List tag is properly set up
	XML_Tag* binary_export_list = entire_xml->Find_Child("BinaryExportList", 1);
	if ( binary_export_list == NULL ) return false;
	for (unsigned int i = 0; i < binary_export_list->Children.size(); ++i)
	{
		bool ValidElement = false;
		ValidElement |= (strcmp(binary_export_list->Children[i]->Get_Name(), "ExportList") == 0);
		ValidElement |= (strcmp(binary_export_list->Children[i]->Get_Name(), "OutputFileName") == 0);
		if (ValidElement == false) return false;
	}

	//  Verify that the Export List tag is properly set up
	XML_Tag* export_list_tag = binary_export_list->Find_Child( "ExportList", 1 );
	if ( export_list_tag == NULL ) return false;
	for (unsigned int i = 0; i < export_list_tag->Children.size(); ++i)
	{
		bool ValidElement = false;
		ValidElement |= (strcmp(export_list_tag->Children[i]->Get_Name(), "char") == 0);
		ValidElement |= (strcmp(export_list_tag->Children[i]->Get_Name(), "short") == 0);
		ValidElement |= (strcmp(export_list_tag->Children[i]->Get_Name(), "int") == 0);
		ValidElement |= (strcmp(export_list_tag->Children[i]->Get_Name(), "float") == 0);
		ValidElement |= (strcmp(export_list_tag->Children[i]->Get_Name(), "double") == 0);
		ValidElement |= (strcmp(export_list_tag->Children[i]->Get_Name(), "string") == 0);
		if (ValidElement == false) return false;
	}

	//  Loop through each Export Element and export each value
	std::ofstream output_stream( binary_export_list->Find_Child("OutputFileName", 1)->Get_Value(), std::ios_base::binary | std::ios_base::trunc);
	for (unsigned int i = 0; i < export_list_tag->Children.size(); ++i)
	{
		ExportValue( output_stream, export_list_tag->Children[i]->Get_Name(), export_list_tag->Children[i]->Get_Value() );
	}

	return true;
}

#endif