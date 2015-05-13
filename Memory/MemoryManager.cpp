#include "MemoryManager.h"

namespace MemoryManager
{
  // IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT 
  //
  // This is the only static memory that you may use, no other global variables may be
  // created, if you need to save data make it fit in MM_pool
  //
  // IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT


  const int MM_POOL_SIZE = 65536;
  char* MM_pool[MM_POOL_SIZE];

  // Initialize set up any data needed to manage the memory pool
  void initializeMemoryManager(void)
  {
    //~ TODO : IMPLEMENT ME
	for (int i = 0; i < MM_POOL_SIZE; i++) MM_pool[i] = 0;
  }

  // return a pointer inside the memory pool
  // If no chunk can accommodate aSize call OnAllocFail()
  void* allocate(int aSize)
  {
    //~ TODO: IMPLEMENT ME

	if (largestFree() == 0) onOutOfMemory();
	
	// Every piece of data is preceded by an unsigned short value indicating how much space it takes. To go through it, 
	// start a value at the beginning, then jump forward the number of bytes that the value is to find the next space.
	// Continue until you find a 0 or reach a point where jumping again would lead you to overshoot the area you are allowed
	// to allocate memory into. When you reach a 0, make sure that allocating the data will not overshoot you either, 
	// and if it won't, return the byte address.

	// Just for simplification, here is an unsigned int to represent the memory location of MM_pool
	char* Pool = (char*)&MM_pool[0];

	for (int i = 0; i < MM_POOL_SIZE * 4; )
	{
		// Check the location that i is at. If it is not zero, jump ahead and continue
		if (((unsigned short)(*(Pool + i))) != 0)
		{
			i += *((unsigned short*)(Pool + i)) + sizeof(unsigned short);
			continue;
		}
		else
		{
			// If allocating would take you past the end of the array, return -1
			if (i + aSize + sizeof(unsigned short) > MM_POOL_SIZE * 4)
			{
				onOutOfMemory();
				return ((void*) -1);
			}

			// If allocating would plow through another allocation, continue;
			bool Continue = false;
			for (int j = 0; j < aSize + (int)sizeof(unsigned short); j += 1)
			{
				if (*((char*)(Pool + i + j)) != 0)
				{
					i += j + (*((unsigned short*)(Pool + i + j)));
					Continue = true;
					break;
				}
			}
			if (Continue) continue;

			// Allocate if you get this far
			(*((unsigned short*)(Pool + i))) = (unsigned short)aSize;
			return ((void*) (((unsigned short*)(Pool + i + sizeof(unsigned short)))));
		}
	}

    return ((void*) 0);
  }

  // Free up a chunk previously allocated
  void deallocate(void* aPointer)
  {
    //~ TODO: IMPLEMENT ME
	char* Location = (char*)aPointer;
	unsigned short Length = (unsigned short)(*(Location - sizeof(unsigned short))) + sizeof(unsigned short);

	for (int i = 0; i < Length; i++)
		(char)(*(Location - sizeof(unsigned short) + i)) = 0;
  }

  //---
  //--- support routines
  //--- 

  // Will scan the memory pool and return the total free space remaining
  int freeRemaining(void)
  {
    //~ TODO: IMPLEMENT ME

	char* Pool = (char*)&MM_pool[0];
	int FreeSpace = 0;
	for (int i = 0; i < MM_POOL_SIZE * sizeof(MM_pool[0]); i++)
	{
		if (*((char*)(Pool + i)) == 0)	FreeSpace += 1;
		else							i += (*((unsigned short*)(Pool + i))) + sizeof(unsigned short) - 1;
	}
	// NOTE: This fails if anything overruns! The string implementation I put together should prevent that 
	// type of overflow but I haven't prevented it from being misused...

    return FreeSpace;
  }

  // Will scan the memory pool and return the largest free space remaining
  int largestFree(void)
  {
    //~ TODO: IMPLEMENT ME
	char* Pool = (char*)&MM_pool[0];

	unsigned int Largest = 0;
	unsigned int Temp = 0;

	for (int i = 0; i < MM_POOL_SIZE * sizeof(MM_pool[0]); i++)
	{
		if (*((unsigned short*)(Pool + i)) == 0)
		{
			Temp = 0;
			for (int j = 0; (*((char*)(Pool + i + j)) == 0) && (i + j < MM_POOL_SIZE * sizeof(MM_pool[0])); j += 1)
				Temp += 1;
			if (Temp > Largest)
				Largest = Temp;
			i += Temp;
		}
	}

    return Largest;
  }

  // will scan the memory pool and return the smallest free space remaining
  int smallestFree(void)
  {
    // TODO: IMPLEMENT ME
	char* Pool = (char*)&MM_pool[0];

	unsigned int Smallest = MM_POOL_SIZE * sizeof(MM_pool[0]);
	unsigned int Temp = MM_POOL_SIZE * sizeof(MM_pool[0]);

	for (int i = 0; i < MM_POOL_SIZE * sizeof(MM_pool[0]); i++)
	{
		if (*((unsigned short*)(Pool + i)) == 0)
		{
			Temp = 0;
			for (int j = 0; (*((char*)(Pool + i + j)) == 0) && (i + j < MM_POOL_SIZE * sizeof(MM_pool[0])); j += 1)
				Temp += 1;
			if (Temp < Smallest)
				Smallest = Temp;
			i += Temp;
		}
	}

    return Smallest;
  }

  //--- OPTIONAL extensions

  // [OPTIONAL] returns true if Allocate(), Deallocate(), FreeRemaining(), LargestFree(), SmallestFree()
  // are thread safe functions.
  bool isTreadsafe(void)
  {
    return false;         // default, change if you wish to add thread safety
  }

  // [OPTIONAL]] returns true if system supports detecting memory overruns
  bool detectsPointerOverruns(void)
  {
    return false;         // default, change if you have a method for detecting pointer overruns
  }
}