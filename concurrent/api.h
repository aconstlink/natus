#pragma once

#if defined( NATUS_BUILD_DLL )

    #if defined( NATUS_CONCURRENT_EXPORT )
    #define NATUS_CONCURRENT_API_C extern "C" __declspec(dllexport)
    #define NATUS_CONCURRENT_API __declspec(dllexport)
    #else
    #define NATUS_CONCURRENT_API_C extern "C" __declspec(dllimport)
    #define NATUS_CONCURRENT_API __declspec(dllimport)
    #endif    

#else 

    #define NATUS_CONCURRENT_API_C
    #define NATUS_CONCURRENT_API

#endif // NATUS_TARGET_OS_WIN
