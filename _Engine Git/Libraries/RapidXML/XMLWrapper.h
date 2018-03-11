#ifndef _XML_WRAPPER_H_
#define _XML_WRAPPER_H_

#include "./rapidxml_utils.hpp"
#include <unordered_map>
#include "./../SimpleMD5/SimpleMD5.h"

typedef rapidxml::file<>				RapidXML_File;
typedef rapidxml::xml_document<>		RapidXML_Doc;
typedef rapidxml::xml_node<>			RapidXML_Node;

class XMLWrapper
{
public:
	static XMLWrapper* Get_Instance(void)
	{
		static XMLWrapper INSTANCE;
		return &INSTANCE;
	}

	const RapidXML_Doc* LoadXMLFile( const char* filename )
	{
		RapidXML_File* newFile = NULL;
		std::string hash = md5( std::string( filename ) );
		
		XMLListType::const_iterator findIter = LoadedXMLList.find( hash );
		if (findIter != LoadedXMLList.end()) newFile = (*findIter).second;
		else newFile = new RapidXML_File( filename );

		RapidXML_Doc* newDoc = new RapidXML_Doc;
		newDoc->parse<0>( newFile->data() );
		
		LoadedXMLList[hash] = newFile;
		return newDoc;
	}

	bool RemoveXMLFile( const char* filename )
	{
		std::string hash = md5( std::string( filename ) );
		XMLListType::const_iterator findIter = LoadedXMLList.find( hash );
		if (findIter != LoadedXMLList.end())
		{
			delete (*findIter).second;
			LoadedXMLList.erase(findIter);
			return true;
		}

		return false;
	}

private:
	XMLWrapper()	{}
	~XMLWrapper()	{}

	typedef std::unordered_map< std::string, RapidXML_File* > XMLListType;
	XMLListType LoadedXMLList;
};

#endif