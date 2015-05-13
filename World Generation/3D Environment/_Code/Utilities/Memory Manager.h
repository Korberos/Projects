#pragma once
#pragma warning(disable : 4996)

#include <fstream>
using namespace std;

class MemoryManager
{
public:
	MemoryManager()
	{
		MemoryInUse		= 0;
		MaxMemoryInUse	= 0;
		Allocations		= 0;
		MaxAllocations	= 0;
	}

	~MemoryManager()
	{
		char String[64];
		fstream F("MemoryOutput.txt", ios_base::out);

		sprintf(String, "Memory Situation:\t%s\n\n", ((MemoryInUse == 0) ? "NO LEAKS FOUND!" : "MEMORY LEAKS FOUND! DEBUG IMMEDIATELY!"));
		F.write(String, (streamsize)(strlen(String)));

		sprintf(String, "Allocations Unfreed:\t%d\n", Allocations);
		F.write(String, (streamsize)(strlen(String)));

		sprintf(String, "Memory Unfreed:\t\t%d byte(s)\n\n", MemoryInUse);
		F.write(String, (streamsize)(strlen(String)));
		
		sprintf(String, "Maximum Allocations:\t%d\n", MaxAllocations);
		F.write(String, (streamsize)(strlen(String)));
		
		sprintf(String, "Maximum Memory Used:\t%d byte(s)\n", MaxMemoryInUse);
		F.write(String, (streamsize)(strlen(String)));

		F.close();
	}

	int	MemoryInUse;
	int	MaxMemoryInUse;

	int	Allocations;
	int MaxAllocations;
};

static MemoryManager MEMORY;

void* operator new(size_t Bytes)
{
	MEMORY.MemoryInUse += (int)Bytes;
	MEMORY.MaxMemoryInUse = max(MEMORY.MaxMemoryInUse, MEMORY.MemoryInUse);

	MEMORY.Allocations += 1;
	MEMORY.MaxAllocations = max(MEMORY.MaxAllocations, MEMORY.Allocations);

	return malloc(Bytes);
}

void operator delete(void* Data)
{
	MEMORY.MemoryInUse -= (int)(_msize(Data));
	MEMORY.Allocations -= 1;

	return free(Data);
}

void* operator new[](size_t Bytes)
{
	MEMORY.MemoryInUse += (int)Bytes;
	MEMORY.MaxMemoryInUse = max(MEMORY.MaxMemoryInUse, MEMORY.MemoryInUse);

	MEMORY.Allocations += 1;
	MEMORY.MaxAllocations = max(MEMORY.MaxAllocations, MEMORY.Allocations);

	return malloc(Bytes);
}

void operator delete[](void* Data)
{
	MEMORY.MemoryInUse -= (int)(_msize(Data));
	MEMORY.Allocations -= 1;

	return free(Data);
}