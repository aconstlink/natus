#pragma once

#if defined( NATUS_DEBUG )

#include <assert.h>
#define natus_assert(x) assert(x)

#else

#define natus_assert(x) if(!(x)){/*do critical output and exit*/}((void)0)

#endif




