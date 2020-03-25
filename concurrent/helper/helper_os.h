#pragma once

#if defined( NATUS_TARGET_OS_WIN )
#include "win32/win32_helper.h"
namespace natus::concurrent
{
    namespace concurrent
    {
        typedef natus::concurrent::win32::helper helper_t ;
    }
}
#elif defined( NATUS_TARGET_OS_LIN )
#include "unix/pthread_helper.h"
namespace natus
{
    namespace concurrent
    {
        typedef natus::concurrent::osunix::pthread_helper helper_t ;
    }
}
#else
#error "Helper not available on this plattform."
#endif
