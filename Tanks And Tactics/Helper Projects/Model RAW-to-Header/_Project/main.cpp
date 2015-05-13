////////////////////////////////////////////////////////////////////////////////
//
//	Name:			Model RAW-to-Header
//
//	Description:	This program will take in a Milkshape 3D RAW file 
//					and convert it to a .h file that can be loaded into 
//					the ModelExporter solution to be altered and used 
//					to export a Tanks and Tactics model file.
//
//	Created:		December 23rd, 2008
//
//	Last Modified:	December 23rd, 2008
//
////////////////////////////////////////////////////////////////////////////////

#include <fstream>
#include "ModelStructure.h"

#define MILKSHAPE_UNIT_TO_TAT_UNIT		0.0018f

char				String[1024];
char				TempString[1024];
unsigned int		Index;
unsigned int		TempIndex;

Model				MODEL;
Model::Model_Part*	CurrentPart;
float				Triangle[3][3];

bool	LoadModelRAW(char* RawFile);
bool	ExportModelH(char* HFile);
void ConvertCharsToFloat(char* String, float* Triangle);

void main(void)
{
	CurrentPart = NULL;

	if (!LoadModelRAW("Input.raw")) return;
	if (!ExportModelH("./../Output.h"))	return;
}

bool LoadModelRAW(char* RawFile)
{
	// Open the input RAW file. Exit the program if unable to access the file
	std::fstream RAW_File(RawFile, std::ios_base::in | std::ios_base::binary);
	if (RAW_File.bad())		return false;
	if (!RAW_File.good())	return false;

	while (!RAW_File.eof())
	{
		RAW_File.getline(String, 1024);
		if ((String[0] >= 65) && (String[0] <= 122))			// The line is a part name
		{
			CurrentPart = MODEL.AddPart();
			strcpy_s(CurrentPart->Name, 32, String); CurrentPart->Name[31] = 0;
			continue;
		}
		else if ((String[0] >= 45) && (String[0] <= 57))	// The line is a part element
		{
			ConvertCharsToFloat(String, &Triangle[0][0]);
			CurrentPart->AddTriangle(&Triangle[0][0]);
		}
	}

	RAW_File.close();
	return true;
}

bool ExportModelH(char* HFile)
{
	// Open the output HEADER file. Exit the program if unable to access the file
	std::fstream Header(HFile, std::ios_base::out);
	if (Header.bad())		return false;
	if (!Header.good())		return false;

	for (unsigned int i = 0; i < MODEL.PartList.size(); i++)
	{
		// Write out the opening definition for the part's vertex list
		sprintf_s(String, 7, "float ");
		Header.write((char*)String, (unsigned int)strlen(String));
		Header.write((char*)MODEL.PartList[i].Name, (unsigned int)strlen(MODEL.PartList[i].Name) - 1);
		sprintf_s(String, 20, "_V[3][%d] = \n{  \n", (unsigned int)MODEL.PartList[i].VertexList.size());
		Header.write((char*)String, (unsigned int)strlen(String));

		// Write out each vertex in rows of three, separated by a comma and space
		for (unsigned int j = 0; j < (unsigned int)MODEL.PartList[i].VertexList.size(); j++)
		{
			sprintf_s(String, 53, "\t%ff, %ff, %ff, \n", MODEL.PartList[i].VertexList[j].Position[0], MODEL.PartList[i].VertexList[j].Position[1], MODEL.PartList[i].VertexList[j].Position[2]);
			Header.write((char*)String, (unsigned int)strlen(String));
		}
		
		// Write the closing bracket for the vertex list
		sprintf_s(String, 7, "};\n\n");
		Header.write((char*)String, (unsigned int)strlen(String));

		// Write out the opening definition for the part's triangle list
		sprintf_s(String, 14, "unsigned int ");
		Header.write((char*)String, (unsigned int)strlen(String));
		Header.write((char*)MODEL.PartList[i].Name, (unsigned int)strlen(MODEL.PartList[i].Name) - 1);
		sprintf_s(String, 20, "_T[3][%d] = \n{  \n", (unsigned int)MODEL.PartList[i].TriangleList.size());
		Header.write((char*)String, (unsigned int)strlen(String));

		// Write out each vertex in rows of three, separated by a comma and space
		for (unsigned int j = 0; j < (unsigned int)MODEL.PartList[i].TriangleList.size(); j++)
		{
			sprintf_s(String, 50, "\t%d, %d, %d, \n", MODEL.PartList[i].TriangleList[j].VertexID[0], MODEL.PartList[i].TriangleList[j].VertexID[1], MODEL.PartList[i].TriangleList[j].VertexID[2]);
			Header.write((char*)String, (unsigned int)strlen(String));
		}

		
		// Write the closing bracket for the triangle list
		sprintf_s(String, 7, "};\n\n");
		Header.write((char*)String, (unsigned int)strlen(String));
	}

	// Write out the count of model parts;
	sprintf_s(String, 1024, "unsigned int ModelPartCount = %d;\n", (unsigned int)MODEL.PartList.size());
	Header.write((char*)String, (unsigned int)strlen(String));

	// Write out the collection of Vertex Lists
	sprintf_s(String, 1024, "float* VertexLists[%d] = {  ", MODEL.PartList.size());
	Index = (unsigned int)strlen(String);
	for (unsigned int i = 0; i < (unsigned int)MODEL.PartList.size(); i++)
	{
		strcpy_s(&String[Index], 2, "&");
		Index += 1;
		for (unsigned int j = 0; j < strlen(MODEL.PartList[i].Name) - 1; j++) String[Index++] = MODEL.PartList[i].Name[j];
		strcpy_s(&String[Index], 9, "_V[0][0]");
		Index += 8;
		if (i < (unsigned int)(MODEL.PartList.size()) - 1)
		{
			strcpy_s(&String[Index], 3, ", ");
			Index += 2;
		}
	}
	strcpy_s(&String[Index], 5, " };\n");
	Header.write((char*)String, (unsigned int)strlen(String));

	// Write out the collection of Triangle Lists
	sprintf_s(String, 1024, "unsigned int* TriangleLists[%d] = {  ", MODEL.PartList.size());
	Index = (unsigned int)strlen(String);
	for (unsigned int i = 0; i < (unsigned int)MODEL.PartList.size(); i++)
	{
		strcpy_s(&String[Index], 2, "&");
		Index += 1;
		for (unsigned int j = 0; j < strlen(MODEL.PartList[i].Name) - 1; j++) String[Index++] = MODEL.PartList[i].Name[j];
		strcpy_s(&String[Index], 9, "_T[0][0]");
		Index += 8;
		if (i < (unsigned int)(MODEL.PartList.size()) - 1)
		{
			strcpy_s(&String[Index], 3, ", ");
			Index += 2;
		}
	}
	strcpy_s(&String[Index], 5, " };\n");
	Header.write((char*)String, (unsigned int)strlen(String));

	// Write out the collection of Vertex List sizes
	sprintf_s(String, 1024, "unsigned int VertexListSizes[%d] = {  ", MODEL.PartList.size());
	Index = (unsigned int)strlen(String);
	for (unsigned int i = 0; i < (unsigned int)MODEL.PartList.size(); i++)
	{
		sprintf_s(TempString, 1024, "%d", (unsigned int)MODEL.PartList[i].VertexList.size());
		TempIndex = (unsigned int)strlen(TempString);
		strcpy_s(&String[Index], TempIndex + 1, TempString);
		Index += TempIndex;
		if (i < (unsigned int)(MODEL.PartList.size()) - 1)
		{
			strcpy_s(&String[Index], 3, ", ");
			Index += 2;
		}
	}
	strcpy_s(&String[Index], 5, " };\n");
	Header.write((char*)String, (unsigned int)strlen(String));

	// Write out the collection of Triangle List sizes
	sprintf_s(String, 1024, "unsigned int TriangleListSizes[%d] = {  ", MODEL.PartList.size());
	Index = (unsigned int)strlen(String);
	for (unsigned int i = 0; i < (unsigned int)MODEL.PartList.size(); i++)
	{
		sprintf_s(TempString, 1024, "%d", (unsigned int)MODEL.PartList[i].TriangleList.size());
		TempIndex = (unsigned int)strlen(TempString);
		strcpy_s(&String[Index], TempIndex + 1, TempString);
		Index += TempIndex;
		if (i < (unsigned int)(MODEL.PartList.size()) - 1)
		{
			strcpy_s(&String[Index], 3, ", ");
			Index += 2;
		}
	}
	strcpy_s(&String[Index], 5, " };\n");
	Header.write((char*)String, (unsigned int)strlen(String));

	// Close the header file
	Header.close();
	return true;
}

void ConvertCharsToFloat(char* String, float* Triangle)
{
	for (int i = 0; i < 9; i++) Triangle[i] = 0.0f;

	unsigned int	CurrentTriangle	= 0;
	unsigned int	CurrentVertex	= 0;
	float			Modifier		= 1.0f;
	bool			HitDecimal		= false;
	bool			Negative		= false;

	for (int i = 0; (String[i] != 13); i++)
	{
		switch (String[i])
		{
		case 32:
			if (Negative) Triangle[CurrentTriangle * 3 + CurrentVertex] *= -1.0;
			Modifier = 1.0f;
			HitDecimal = false;
			Negative = false;

			CurrentVertex += 1;
			if (CurrentVertex == 3)
			{
				i += 2;
				CurrentVertex = 0;
				CurrentTriangle += 1;
			}
			break;

		case 45:
			if (Negative) return;
			Negative = true;
			continue;
			break;

		case 46:
			if (HitDecimal) return;
			HitDecimal = true;
			break;

		case 48:
		case 49:
		case 50:
		case 51:
		case 52:
		case 53:
		case 54:
		case 55:
		case 56:
		case 57:
			Triangle[CurrentTriangle * 3 + CurrentVertex] *= (HitDecimal ? 1.0f : 10.0f);
			Modifier *= (HitDecimal ? 0.1f : 1.0f);
			Triangle[CurrentTriangle * 3 + CurrentVertex] += (float)(String[i] - 48) * Modifier;
			break;
		}
	}

	if (Negative) Triangle[CurrentTriangle * 3 + CurrentVertex] *= -1.0;
	for (int j = 0; j < 9; j++) Triangle[j] *= MILKSHAPE_UNIT_TO_TAT_UNIT;
}