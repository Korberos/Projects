///////////////////////////////////////////////////////////////////////////
//
//	File:		XML_File.h
//
//	Purpose:	Contains functionality to load in an XML file and
//				confirm or deny valid structure, as well as allowing
//				for the file to exist as a class.
//
//	Version:	0003
//
//	Notes:		
//
///////////////////////////////////////////////////////////////////////////

#ifndef _XML_FILE_
#define _XML_FILE_

#include <fstream>
#include <stack>
#include <vector>

#define MAX_LINE_LENGTH		2048

////////////////////////////////////////
// Classes
////////////////////////////////////////
class XML_Tag
{
private:
	char* Name;
	char* Value;

public:
	XML_Tag();
	~XML_Tag();
	bool Define_Tag_Name( char* tag_name );
	bool Define_Tag_Value( char* tag_value );
	bool Is_Same_Tag( char* tag_name );
	
	inline char*	Get_Name( void )		{ return Name; }
	inline char*	Get_Value( void )		{ return Value; }

	XML_Tag*				Parent;

	typedef std::vector<XML_Tag*> ChildListType;
	typedef ChildListType::iterator ChildListIterType;
	ChildListType	Children;

	std::vector< std::pair<std::string, std::string> > Elements;
	
	XML_Tag* Find_Child( char* string, unsigned int depth );
	std::string Get_Element( std::string );
};

class XML_File
{
	//~  Member Functions
	private:
		XML_File();
		
		bool Load_XML_Data( const char* xml_file );
		void Iterate_Whitespace(  char* string, unsigned int& index );
		bool Add_XML_Tag(  char* string, unsigned int& index );
		bool Define_Tag_Value( char* string, unsigned int& index );
		bool Remove_XML_Tag( char* string, unsigned int& index );
		bool Add_XML_Version_Info( char* string, unsigned int& index, char* version_string );
		bool Begin_Comment( char* string, unsigned int& index , bool& within_comment );
		bool Iterate_Comment( char* string, unsigned int& index , bool& within_comment  );
		bool End_Of_XML_Line( char* string, unsigned int& index );

	public:
		static XML_File* Load_XML_File( const char* xml_file );
		~XML_File();

		inline XML_Tag* Get_Encompassing_Tag( void )			{ return &Encompassing_Tag; }

	//~  Member Variables
	private:
		std::stack<XML_Tag*>		XML_Tag_Stack;
		std::vector<XML_Tag*>		XML_Tag_List;
		XML_Tag						Encompassing_Tag;
		XML_Tag*					Current_Tag;

	public:
};

#endif