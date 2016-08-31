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
/*  File:       MegaFile.h                                       */
/*                                                               */
/*  Purpose:    This file contains the functionality to load     */
/*              in or save out an archived, encrypted file.      */
/*                                                               */
/*  Created:    01/20/2009                                       */
/*  Last Edit:  04/18/2009                                       */
/*****************************************************************/

#include <vector>
#include <string>
#include <fstream>

class MegaFile
{
public:
	MegaFile();
	~MegaFile();

	bool	SaveMegaFile(std::string mega_file);
	bool	LoadMegaFile(std::string mega_file);

	bool	AddFile(std::string file_name);
	bool	RemoveFile(std::string file_name);

	bool	ExtractFile(std::string mega_file, std::string file_name, std::string save_name = "");

private:
	class EmbeddedFile
	{
	public:
		std::string		FileName;
		unsigned int	FileSize;
		unsigned int	ByteOffset;

		void operator=(const EmbeddedFile other)
		{
			FileName = other.FileName;
			FileSize = other.FileSize;
			ByteOffset = other.ByteOffset;
		}
	};

	unsigned int HeaderSize;
	std::vector<EmbeddedFile>	FileList;

	void XOR_Encryption(char* byte_array, unsigned int array_size);
};

MegaFile::MegaFile()
{

}

MegaFile::~MegaFile()
{

}

bool MegaFile::SaveMegaFile(std::string mega_file)
{
	if (mega_file == "") return false;
	if (FileList.size() == 0) return false;

	std::ofstream MegaFile(mega_file.c_str(), std::ios_base::binary);
	if (MegaFile.bad() || !MegaFile.good())
	{
		MegaFile.close();
		return false;
	}

	unsigned int FileCount = FileList.size();
	HeaderSize = 0;

	// Write out the Header
	{
		MegaFile.write((char*)&HeaderSize, sizeof(HeaderSize));
		MegaFile.write((char*)&FileCount, sizeof(FileCount));

		HeaderSize = sizeof(HeaderSize) + sizeof(FileCount); 

		unsigned int ByteOffset = 0;

		for (unsigned int i = 0; i < FileCount; ++i)
		{
			unsigned int StringLength = FileList[i].FileName.size();
			MegaFile.write((char*)&StringLength, sizeof(StringLength));

			MegaFile.write((char*)FileList[i].FileName.c_str(), StringLength);

			MegaFile.write((char*)&FileList[i].FileSize, sizeof(FileList[i].FileSize));

			MegaFile.write((char*)&ByteOffset, sizeof(ByteOffset));
			ByteOffset += FileList[i].FileSize;

			HeaderSize += sizeof(StringLength) + StringLength + sizeof(FileList[i].FileSize) + sizeof(ByteOffset);
		}
	}
	
	// Write each file
	for (unsigned int i = 0; i < FileCount; ++i)
	{
		std::ifstream InputFile(FileList[i].FileName.c_str(), std::ios_base::binary);
		if (InputFile.bad() || !InputFile.good())
		{
			InputFile.close();
			MegaFile.close();
			return false;
		}

		char* FileChunk = new char[FileList[i].FileSize];
		InputFile.read((char*)FileChunk, FileList[i].FileSize);
		XOR_Encryption(FileChunk, FileList[i].FileSize);
		MegaFile.write((char*)FileChunk, FileList[i].FileSize);
		delete [] FileChunk;

		InputFile.close();
	}

	// Go back and write the header size
	MegaFile.seekp(0, MegaFile.beg);
	MegaFile.write((char*)&HeaderSize, sizeof(HeaderSize));

	MegaFile.close();
	return true;
}

bool MegaFile::LoadMegaFile(std::string mega_file)
{
	if (mega_file == "") return false;

	std::ifstream MegaFile(mega_file.c_str(), std::ios_base::binary);
	if (MegaFile.bad() || !MegaFile.good())
	{
		MegaFile.close();
		return false;
	}

	FileList.clear();

	// Read in the Header
	{
		unsigned int HeaderSize;
		unsigned int FileCount = 0;
		MegaFile.read((char*)&HeaderSize, sizeof(HeaderSize));
		MegaFile.read((char*)&FileCount, sizeof(FileCount));

		for (unsigned int i = 0; i < FileCount; ++i)
		{
			unsigned int StringLength = 0;
			MegaFile.read((char*)&StringLength, sizeof(StringLength));

			EmbeddedFile NewFile;
			char CharacterIn;
			for (unsigned int j = 0; j < StringLength; ++j)
			{
				MegaFile.read((char*)&CharacterIn, sizeof(CharacterIn));
				NewFile.FileName += CharacterIn;
			}

			MegaFile.read((char*)&NewFile.FileSize, sizeof(NewFile.FileSize));
			MegaFile.read((char*)&NewFile.ByteOffset, sizeof(NewFile.ByteOffset));

			FileList.push_back(NewFile);
		}
	}

	MegaFile.close();
	return true;
}

bool MegaFile::AddFile(std::string mega_file)
{
	if (mega_file == "") return false;

	std::ifstream AddFile(mega_file.c_str(), std::ios_base::binary);
	if (AddFile.bad() || !AddFile.good())
	{
		AddFile.close();
		return false;
	}

	unsigned int Begin = AddFile.tellg();
	AddFile.seekg(0, std::ios::end);
	unsigned int End = AddFile.tellg();
	AddFile.close();

	EmbeddedFile NewFile;
	NewFile.FileName = mega_file;
	NewFile.FileSize = End - Begin;

	FileList.push_back(NewFile);

	return true;
}

bool MegaFile::RemoveFile(std::string mega_file)
{
	if (mega_file == "") return false;
	if (FileList.size() == 0) return false;

	for (unsigned int i = 0; i < FileList.size(); ++i)
	{
		if (FileList[i].FileName == mega_file)
		{
			FileList[i] = FileList[FileList.size() - 1];
			FileList.pop_back();
			return true;
		}
	}

	return false;
}

bool MegaFile::ExtractFile(std::string mega_file, std::string file_name, std::string save_name)
{
	if (file_name == "") return false;
	if (!LoadMegaFile(mega_file)) return false;
	if (FileList.size() == 0) return false;

	for (unsigned int i = 0; i < FileList.size(); ++i)
	{
		if (FileList[i].FileName == file_name)
		{
			std::ifstream MegaFile(mega_file.c_str(), std::ios_base::binary);
			if (MegaFile.bad() || !MegaFile.good())
			{
				MegaFile.close();
				return false;
			}

			std::ofstream OutputFile(save_name == "" ? file_name.c_str() : save_name.c_str(), std::ios_base::binary | std::ios_base::trunc);
			if (OutputFile.bad() || !OutputFile.good())
			{
				OutputFile.close();
				MegaFile.close();
				return false;
			}

			char* FileChunk = new char[FileList[i].FileSize];
			MegaFile.seekg(FileList[i].ByteOffset + HeaderSize);
			MegaFile.read((char*)FileChunk, FileList[i].FileSize);
			XOR_Encryption(FileChunk, FileList[i].FileSize);
			OutputFile.write((char*)FileChunk, FileList[i].FileSize);
			OutputFile.close();
			MegaFile.close();

			return true;
		}
	}

	return false;
}

void MegaFile::XOR_Encryption(char* byte_array, unsigned int array_size)
{
	for (unsigned int i = 0; i < array_size; ++i) byte_array[i] ^= 255;
}