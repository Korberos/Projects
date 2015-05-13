#include "XML_File.h"

#include "DebugTools.h"

////////////////////////////////////////
// Functions (XML_Tag)
////////////////////////////////////////
XML_Tag::XML_Tag()
{
	Name = NULL;
	Value = NULL;

	Parent = NULL;
}

XML_Tag::~XML_Tag()
{
	if (Name != NULL) delete [] Name;
	if (Value != NULL) delete [] Value;
}

bool XML_Tag::Define_Tag_Name( char* tag_name )
{
	FAIL_IF ( !tag_name ) return false;

	if ( Name )
	{
		delete [] Name;
	}
	Name = new char[strlen(tag_name) + 1];
	strcpy_s(Name, strlen(tag_name) + 1, tag_name);

	return true;
}

bool XML_Tag::Define_Tag_Value( char* tag_value )
{
	FAIL_IF ( !tag_value ) return false;

	if ( Value )
	{
		delete [] Value;
	}
	Value = new char[strlen(tag_value) + 1];
	strcpy_s(Value, strlen(tag_value) + 1, tag_value);

	return true;
}

bool XML_Tag::Is_Same_Tag( char* tag_name )
{
	FAIL_IF (!tag_name) return false;

	if (strcmp(tag_name, Name) == 0) return true;
	else return false;
}

XML_Tag* XML_Tag::Find_Child( char* string, unsigned int depth )
{
	std::vector<XML_Tag*> tag_iterations;
	std::vector<XML_Tag*> new_tags;
	tag_iterations.push_back(this);

	for (unsigned int i = 0; i < depth; ++i)
	{
		if (tag_iterations.size() == 0) return NULL;

		for (unsigned int j = 0; j < tag_iterations.size(); ++j)
			for (unsigned int k = 0; k < tag_iterations[j]->Children.size(); ++k)
				new_tags.push_back(tag_iterations[j]->Children[k]);

		tag_iterations.clear();

		for (unsigned int j = 0; j < new_tags.size(); ++j)
			tag_iterations.push_back(new_tags[j]);
	}

	for (unsigned int i = 0; i < tag_iterations.size(); ++i)
		if (strcmp(tag_iterations[i]->Get_Name(), string) == 0)
			return tag_iterations[i];

	return NULL;
}


std::string XML_Tag::Get_Element( std::string name )
{
	FAIL_IF (name.empty()) return std::string("");

	for (unsigned int i = 0; i < Elements.size(); ++i)
		if (Elements[i].first.compare(name) == 0)
			return Elements[i].second;

	FAIL_IF ( true ) return std::string("");
}


////////////////////////////////////////
// Functions (XML_File)
////////////////////////////////////////
XML_File::XML_File()
{
	Current_Tag = &Encompassing_Tag;
}

XML_File::~XML_File()
{
	while ( !XML_Tag_List.empty() )
	{
		XML_Tag* delete_tag = XML_Tag_List.back();
		XML_Tag_List.pop_back();
		delete delete_tag;
	}
}

XML_File* XML_File::Load_XML_File( const char* xml_file )
{
	// Allocate a new XML file. If one can not be allocated, return false
	XML_File* new_xml_file = new XML_File;
	FAIL_IF ( !new_xml_file ) return NULL;

	FAIL_IF ( !new_xml_file->Load_XML_Data( xml_file ) )
	{
		delete new_xml_file;
		return NULL;
	}

	return new_xml_file;
}

bool XML_File::Load_XML_Data( const char* xml_file )
{
	FAIL_IF ( !xml_file ) return false;
	std::ifstream file_stream( xml_file , std::ios_base::binary );
	FAIL_IF ( file_stream.bad() || !file_stream.good() )
	{
		file_stream.close();
		return false;
	}

	//  Initialize variables for later use
	unsigned int	row_index = 0;
	unsigned int	col_index = 0;
	bool			within_comment = false;
	char			xml_version[16] = { "" };
	char			line_string[MAX_LINE_LENGTH] = { "" };

	while ( !file_stream.eof() )
	{
		// Iterate the row index and set the column back to 0
		row_index += 1;
		col_index = 0;

		//  Get the next line and iterate through any opening whitespace
		file_stream.getline( line_string, MAX_LINE_LENGTH );
		Iterate_Whitespace( line_string, col_index );
		if ( line_string[col_index] == 0 || line_string[col_index] == 13 )
		{
			continue;
		}
		
		//  If we are inside of a comment, continue iterating until the comment is closed
		if ( within_comment )
		{
			FAIL_IF ( !Iterate_Comment( line_string, col_index, within_comment ) ) return false;
			else
			{
				Iterate_Whitespace( line_string , col_index );
				if ( line_string[col_index] == 0 || line_string[col_index] == 13 ) continue;
			}
		}

		//  If the first character is not a '<' then we know that the XML is not valid
		FAIL_IF ( line_string[col_index] != '<' ) return false;

		//  Based on the next character, decide which action to take (tag opening, tag closing, commenting, or version defining)
		bool next_line = false;
		while ( line_string[col_index] == '<' )
		{
			++col_index;
			switch ( line_string[col_index] )
			{
			case '/':
				FAIL_IF ( !Remove_XML_Tag( line_string , col_index ) ) return false;
				next_line = true;
				break;

			case '?':
				FAIL_IF ( !Add_XML_Version_Info( line_string , col_index , xml_version) ) return false;
				next_line = true;
				break;

			case '!':
				FAIL_IF ( !Begin_Comment( line_string , col_index , within_comment ) ) return false;
				next_line = true;
				break;

			default:
				FAIL_IF (!Add_XML_Tag( line_string, col_index ) ) return false;
				next_line = false;
				
				// If the newly added tag is the end of the line, continue to the next line
				Iterate_Whitespace( line_string , col_index );
				if ( line_string[col_index] == 0 || line_string[col_index] == 13 ) next_line = true;

				//  We should now take in whatever is within the tag, since a value is present
				Define_Tag_Value( line_string, col_index );

				break;
			}
			if (next_line == true) break;
		}
		if (next_line == true) continue;
		
		//  We've reached the '<' that SHOULD be the matching close tag to the newly 
		//  added tag. If not, the XML is not valid
		Iterate_Whitespace( line_string , col_index );
		FAIL_IF ( line_string[++col_index] != '/' ) return false;

		//  Attempt to remove a tag based on the current string
		FAIL_IF ( !Remove_XML_Tag( line_string, col_index ) ) return false;

		//  If you have not reached the end of the line, the XML is not valid
		FAIL_IF ( !End_Of_XML_Line( line_string, col_index ) ) return false;
	}

	//  If we reach the end of the file and there is still a tag in the stack, then
	//  we know that the XML is not valid
	FAIL_IF ( !XML_Tag_Stack.empty() )
	{
		FAIL_IF (within_comment) return false; // Separate return just to notify any debuggers that this is the reason... a comment never ended
		return false;
	}

	//  If we have reached this far, we know that the XML is valid
	file_stream.close();
	return true;
}

void XML_File::Iterate_Whitespace( char* string, unsigned int& index )
{
	while (true)
	{
		switch (string[index])
		{
		case 0:
		case '\t':
		case ' ':
			++index;
			break;

		default:
			return;
		}
	}
}


bool XML_File::Add_XML_Tag( char* string, unsigned int& index )
{
	unsigned int	tag_length;
	unsigned int	element_length;
	char			tag_name[MAX_LINE_LENGTH];
	char			element_string[MAX_LINE_LENGTH];

	// If there is no ending to this tag, it is not XML Valid
	char* end_test = strchr(&string[index], '>');
	if (end_test == NULL) return false;

	// Check for a space in the tag name (illegal for now, need to implement things like this <Name extra="lolol">)
	char* space_test = strchr(&string[index], ' ');
	if (space_test != NULL && space_test > end_test) space_test = NULL;

	// Set the value to stop reading (at the end of the first string)
	char* iteration_offset = space_test ? space_test : strchr(&string[index], '>');
	if (iteration_offset == NULL) return false;

	//  Copy over the characters within the value.
	tag_length = unsigned int(iteration_offset - &string[index]);
	strncpy_s(tag_name, tag_length + 1, &string[index], tag_length);
	tag_name[tag_length] = 0;
	index += tag_length;

	//  Define a new XML_Tag with the information, and add it to the stack and list
	XML_Tag* new_tag = new XML_Tag;
	new_tag->Define_Tag_Name(tag_name);
	XML_Tag_Stack.push(new_tag);
	XML_Tag_List.push_back(new_tag);

	Current_Tag->Children.push_back( new_tag );
	new_tag->Parent = Current_Tag;
	Current_Tag = new_tag;

	//  If there are added elements in the tag, take them in
	while (space_test)
	{
		std::pair<std::string, std::string> new_element;

		++index;
		char* start_element = &string[index];
		char* end_element   = strchr(&string[index], '=');
		if ( !end_element ) return false;

		element_length = unsigned int(end_element - start_element);
		strncpy_s(element_string, element_length + 1, start_element, element_length);
		new_element.first = std::string(element_string);
		
		index += element_length + 2;
		space_test = strchr(strchr(&string[index], '\"'), ' ');
		if (space_test != NULL && space_test > end_test) space_test = NULL;
		start_element = &string[index];
		end_element   = space_test ? space_test - 1 : end_test - 1;
		element_length = unsigned int(end_element - start_element);
		strncpy_s(element_string, element_length + 1, start_element, element_length);
		new_element.second = std::string(element_string);

		new_tag->Elements.push_back(new_element);
		index += element_length + 1;
	}

	//  If the next character is not a '>' the the XML is not valid
	FAIL_IF (string[index] != '>') return false;
	++index;

	return true;
}

bool XML_File::Define_Tag_Value( char* string, unsigned int& index )
{
	int	tag_length;
	char	tag_value[MAX_LINE_LENGTH];

	//  If we're at this point, we need to take in the value within the newly added
	//  tag and then check for a closing of the tag (if the XML is valid)
	char* iteration_offset = strchr( &string[index], '<');
	if (iteration_offset == NULL) return false;
	while (*(iteration_offset - 1) == ' ')
	{
		--iteration_offset;
		if (iteration_offset == &string[index]) return false;
	}

	tag_length = int(iteration_offset - &string[index]);
	if ( tag_length <= 0 ) return false;
	strncpy_s(tag_value, tag_length + 1, &string[index], tag_length);
	tag_value[tag_length] = 0;
	Current_Tag->Define_Tag_Value( tag_value );
	index += tag_length;

	return true;
}

bool XML_File::Remove_XML_Tag( char* string, unsigned int& index )
{
	unsigned int	tag_length;
	char			tag_name[MAX_LINE_LENGTH];

	// Increment the Character Index to reach the tag name
	++index;

	//  Copy over the characters until a '>' is reached. If the end of the file
	//  or a null character is reached first, we know the XML is not valid.
	memset(tag_name, NULL, MAX_LINE_LENGTH);
	for (tag_length = 0; string[index + tag_length] != '>'; ++tag_length)
	{
		if (string[index + tag_length] == 0) return false;
		if (index + tag_length >= MAX_LINE_LENGTH) return false;

		tag_name[tag_length] = string[index + tag_length];
	}
	tag_name[tag_length] = 0;
	index += tag_length;

	//  Test if the top tag in the stack is the same tag. If not, we know that 
	//  the XML is not valid
	FAIL_IF (!XML_Tag_Stack.top()->Is_Same_Tag(tag_name)) return false;
	else
	{
		XML_Tag* remove_tag = XML_Tag_Stack.top();
		Current_Tag = remove_tag->Parent;
		XML_Tag_Stack.pop();
	}

	//  If the next character is not a '>' then the XML is not valid
	if (string[index] != '>') return false;
	++index;

	return true;
}

bool XML_File::Add_XML_Version_Info( char* string, unsigned int& index, char* version_string )
{
	//  Check for the "xml version"
	++index;
	int difference = strncmp(&string[index], "xml version=\"", 13);
	if (difference != 0) return false;
	
	index += 13;
	for (unsigned int i = 0; i < 16; ++i)
	{
		if (string[index + i] != '\"')
		{
			version_string[i] = string[index + i];
			continue;
		}
		else
		{
			version_string[i] = 0;
			index += i + 1;
			break;
		}
		if (i == 15) return false;
	}

	// Check for "?>". If it does not appear here, we know the XML is not valid.
	if (strncmp(&string[index], "?>", 2) != 0) return false;
	return true;
}

bool XML_File::Begin_Comment( char* string, unsigned int& index, bool& within_comment )
{
	//  Check that the second '-' symbol exists
	if (string[++index] != '-') return false;
	if (string[++index] != '-') return false;

	within_comment = true;
	return Iterate_Comment( string, index , within_comment );
}

bool XML_File::Iterate_Comment( char* string, unsigned int& index , bool& within_comment )
{
	//  Find if there is a '>' within the line. If there is, ensure that it is a
	//  comment end. Otherwise, skip past it and try again. Once you find that there
	//  are no '>' in the line anymore, return true to go to the next line.
	char* ending_bracket = strchr(&string[index], '>');

	while ( ending_bracket )
	{
		if ( (*(ending_bracket - 1) == '-') && (*(ending_bracket - 2) == '-') )
		{
			index += (unsigned int)(ending_bracket - &string[index]) + 1;
			within_comment = false; 
			return true;
		}
		else
		{
			ending_bracket = strchr((ending_bracket + 1), '>');
		}
	}
	index = (unsigned int)(strlen( string ) - 1);
	return true;
}

bool XML_File::End_Of_XML_Line( char* string, unsigned int& index )
{
	for (unsigned int i = index; i < MAX_LINE_LENGTH; ++i)
	{
		if (string[i] == ' ') continue;
		if (string[i] == 0) return true;
	}

	return false;
}