#ifndef _DEBUG_TOOLS_
#define _DEBUG_TOOLS_

void Assert_Handler( const void *expression, const void *file_name, unsigned line_number );

#define ASSERT(exp)	(void) ((exp) || (Assert_Handler(#exp, __FILE__, __LINE__), 0) )
#define FAIL_IF(exp)	if ((exp) && (Assert_Handler(#exp, __FILE__, __LINE__), true))
#define ENFORCED_IF(exp) if ((exp) || (Assert_Handler(#exp, __FILE__, __LINE__), false))

#endif