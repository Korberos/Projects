#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <iostream>

#include "MemoryManager.h"

// NOTE: This should replace the string overwriting function, and will prevent run-over
bool WriteStringToMM(char* Location, char* String)
{
	unsigned short MaxLength = (unsigned short)(*(Location - 2));
	
	for (int i = 0; i < MaxLength - 1; i += 1)
		Location[i] = String[i];
	Location[MaxLength - 1] = 0;

	if (MaxLength < strlen(String) + 1) return false;
	return true;
}

void main(void)
{
  using namespace MemoryManager;

  initializeMemoryManager();

  long* int_pointer;
  char* string_pointer;

  std::cout << "Free memory = " << freeRemaining() << std::endl;

  int_pointer = (long *) allocate(sizeof(long));
  string_pointer = (char*) allocate(33);

  *int_pointer = 0xDEADBEEF;
  WriteStringToMM(string_pointer, "It was the best of times, it was the worst of times");

  // NOTE: If you write that a char* equals a string like this, it overwrites what address the char* points to...
  //string_pointer = "It was the best of times, it was the worst of times";

  std::cout << "Free memory = " << freeRemaining() << std::endl;

  deallocate(int_pointer);

  std::cout << "Largest Free Area = " << largestFree() << std::endl;
  std::cout << "Smallest Free Area = " << smallestFree() << std::endl;

  deallocate(string_pointer);

  std::cout << "Largest Free Area = " << largestFree() << std::endl;
  std::cout << "Smallest Free Area = " << smallestFree() << std::endl;
}

namespace MemoryManager
{
  void onOutOfMemory(void)
  {
    std::cerr << "Memory pool out of memory" << std::endl;
    exit( 1 );
  }

  // Call if a pointer over run condition is detected
  void onOverrunDetected(void)
  {
    std::cerr << "Pointer overrun detected" << std::endl;
    exit( 1 );
  }

  // call for any other error condition, providing meaningful error messages are appreciated
  void onIllegalOperation(const char* fmt,...)
  {
    if ( fmt == NULL )
    {
      std::cerr << "Unknown illegal operation" << std::endl;
      exit( 1 );
    }
    else
    {
      char	buf[8192];

      va_list argptr;
      va_start (argptr,fmt);
      vsprintf (buf,fmt,argptr);
      va_end (argptr);

      std::cerr << "Illegal operation: \"" << buf << "\"" << std::endl;
      exit( 1 );
    }
  }
}

