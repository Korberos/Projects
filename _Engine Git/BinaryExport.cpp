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
/*  Purpose:    This file contains functions that will take      */
/*              in an XML file and export binary data values     */
/*              which can then be loaded back easily. (Note:     */
/*              this is a preliminary for what game types will   */
/*              do to save data to the machine.                  */
/*                                                               */
/*  Created:    06/01/2010                                       */
/*  Last Edit:  06/15/2010                                       */
/*****************************************************************/

#ifndef _BINARY_EXPORT_
#define _BINARY_EXPORT_

#include <fstream>
#include "Libraries\RapidXML\XMLWrapper.h"
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

void ExportValue( std::ofstream& output_stream, const char* type, const char* value )
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

	const RapidXML_Doc* exportListDoc = XMLWrapper::Get_Instance()->LoadXMLFile( export_list_file );
	if ( exportListDoc == NULL ) return false;

	const RapidXML_Node* mainNode = exportListDoc->first_node();
	if (mainNode->first_node() == NULL || mainNode->first_node()->next_sibling() != NULL) return false;

	//  Verify that the Binary Export List tag is properly set up
	const RapidXML_Node* binaryExportListNode = mainNode->first_node( "BinaryExportList" );
	if ( binaryExportListNode == NULL ) return false;
	for ( const RapidXML_Node* exportIter = binaryExportListNode->first_node(); exportIter != NULL; exportIter = exportIter->next_sibling() )
	{
		bool ValidElement = false;
		ValidElement |= (strcmp( exportIter->name(), "ExportList" ) == 0 );
		ValidElement |= (strcmp( exportIter->name(), "OutputFileName" ) == 0 );
		if (ValidElement == false) return false;
	}

	//  Verify that the Export List tag is properly set up
	const RapidXML_Node* exportListNode = binaryExportListNode->first_node( "ExportList" );
	if ( exportListNode == NULL ) return false;
	for ( const RapidXML_Node* exportIter = exportListNode->first_node(); exportListNode != NULL; exportListNode = exportListNode->next_sibling() )
	{
		bool ValidElement = false;
		ValidElement |= ( strcmp( exportIter->name(), "char" ) == 0 );
		ValidElement |= ( strcmp( exportIter->name(), "short" ) == 0 );
		ValidElement |= ( strcmp( exportIter->name(), "int" ) == 0 );
		ValidElement |= ( strcmp( exportIter->name(), "float" ) == 0 );
		ValidElement |= ( strcmp( exportIter->name(), "double" ) == 0 );
		ValidElement |= ( strcmp( exportIter->name(), "string" ) == 0 );
		if (ValidElement == false) return false;
	}

	//  Loop through each Export Element and export each value
	std::ofstream output_stream( binaryExportListNode->first_node( "OutputFileName")->value(), std::ios_base::binary | std::ios_base::trunc );
	for ( const RapidXML_Node* exportIter = exportListNode->first_node(); exportListNode != NULL; exportListNode = exportListNode->next_sibling() )
	{
		ExportValue( output_stream, exportIter->name(), exportIter->value() );
	}

	return true;
}

#endif