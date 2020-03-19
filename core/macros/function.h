#pragma once

#include "common.h"

#if defined( NATUS_COMPILER_MSC )

    #define NATUS_FUNC __FUNCTION__
    #define NATUS_LINE __LINE__

#elif defined( NATUS_COMPILER_GNU )

    #define NATUS_FUNC "Func"
    #define NATUS_LINE __LINE__

#else

    #define NATUS_FUNC "NATUS_FUNC" 
    #define NATUS_LINE 0

#endif




