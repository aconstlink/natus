#pragma once

#include "../../typedefs.h"

#include <windows.h>

namespace natus
{
    namespace concurrent
    {
        namespace win32
        {
            const DWORD MS_VC_EXCEPTION = 0x406D1388;

            #pragma pack(push,8)
            typedef struct tagTHREADNAME_INFO
            {
                DWORD dwType; // Must be 0x1000.
                LPCSTR szName; // Pointer to name (in user addr space).
                DWORD dwThreadID; // Thread ID (-1=caller thread).
                DWORD dwFlags; // Reserved for future use, must be zero.
            } THREADNAME_INFO;
            #pragma pack(pop)

            class helper
            {
            public: // compile time interface

                /// set caller thread's name 
                static void_t set_thread_name( ::std::string const& name )
                {
                    set_thread_name( DWORD( -1 ), name ) ;
                }

                /// set caller thread's name 
                static void_t set_thread_name( natus::concurrent::thread_ref_t th, ::std::string const& name )
                {
                    DWORD id = GetThreadId( (HANDLE) th.native_handle() ) ;
                    set_thread_name( id, name ) ;
                }

            private: // win32 specific

                static void_t set_thread_name( DWORD id, ::std::string const& name )
                {
                    THREADNAME_INFO info;
                    info.dwType = 0x1000;
                    info.szName = name.c_str();
                    info.dwThreadID = id;
                    info.dwFlags = 0;

                    
                    //__try
                    {
                        RaiseException( MS_VC_EXCEPTION, 0, sizeof( info ) / sizeof( ULONG_PTR ), ( ULONG_PTR* ) &info );
                    }
                    //__except( EXCEPTION_EXECUTE_HANDLER )
                    {
                    }
                    
                }
            };
        }
    }
}
