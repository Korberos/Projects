////////////////////////////////////////////////////////////////////////////////
//
//	Name:			ModelCreator
//
//	Description:	This project solution will take in a header file
//					and allow direct additions to be made to add in
//					texture coordinates, animations, etc.
//
//	Created:		December 24th, 2008
//
//	Last Modified:	December 24th, 2008
//
////////////////////////////////////////////////////////////////////////////////

#include "Tank_2.h"

#include <fstream>

unsigned int	UINT;
char			STRING[32];

float			TexCoord = 0.0f;

void main(void)
{
	std::fstream OUT("./../Tech Demo/Output.ttmodel", std::ios_base::binary | std::ios_base::out);
	if (OUT.bad())		return;
	if (!OUT.good())	return;

	// Output the part count
	OUT.write((char*)&ModelPartCount, sizeof(ModelPartCount));

	// Loop through each part in the collection
	for (unsigned int i = 0; i < ModelPartCount; i++)
	{
		// For the current part, output the number of vertices
		OUT.write((char*)&VertexListSizes[i], sizeof(VertexListSizes[i]));
	
		// For the current part, output the vertices
		for (unsigned int j = 0; j < VertexListSizes[i]; j++)
		{
			OUT.write((char*)&VertexLists[i][j * 3], sizeof(VertexLists[i][j]) * 3);
			OUT.write((char*)&TexCoord, sizeof(TexCoord));
			OUT.write((char*)&TexCoord, sizeof(TexCoord));
		}

		// For the current part, output the number of triangles
		OUT.write((char*)&TriangleListSizes[i], sizeof(TriangleListSizes[i]));

		// For the current part, output the triangles
		for (unsigned int j = 0; j < TriangleListSizes[i]; j++)
		{
			OUT.write((char*)&TriangleLists[i][j * 3], sizeof(TriangleLists[i][j]) * 3);
		}

		// Output the texture name if it has one (TEMPORARILY NULL)
		STRING[0] = 0;
		OUT.write((char*)STRING, sizeof(char) *  32);
	}
	
	// Output the animation count (TEMPORARILY NULL)
	UINT = 0;
	OUT.write((char*)&UINT, sizeof(UINT));

	OUT.close();
}