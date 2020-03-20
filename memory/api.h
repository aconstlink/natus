#pragma once

#if defined( NATUS_BUILD_DLL )

    #if defined( NATUS_MEMORY_EXPORT )
    #define NATUS_MEMORY_API_C extern "C" __declspec(dllexport)
    #define NATUS_MEMORY_API __declspec(dllexport)
    #else
    #define NATUS_MEMORY_API_C extern "C" __declspec(dllimport)
    #define NATUS_MEMORY_API __declspec(dllimport)
    #endif    

#else 

    #define NATUS_MEMORY_API_C
    #define NATUS_MEMORY_API

#endif // NATUS_TARGET_OS_WIN

