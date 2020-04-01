#pragma once

#include "../iwindow_handle.h"

#include <windows.h>

namespace natus
{
    namespace application
    {
        namespace win32
        {
            class NATUS_APPLICATION_API win32_window_handle : public iwindow_handle
            {
                natus_this_typedefs( win32_window_handle ) ;

            private:

                HWND _handle = NULL ;

            public:

                win32_window_handle( void_t ) ;
                win32_window_handle( HWND handle ) ;
                win32_window_handle( this_rref_t rhv ) ;
                virtual ~win32_window_handle( void_t ) ;

            public:

                this_ref_t operator = ( this_rref_t ) ;

            public:

                HWND set_handle( HWND ) ;
                HWND get_handle( void_t ) ;
                bool_t is_valid( void_t ) ;
            };
            natus_typedefs( win32_window_handle, win32_window_handle ) ;
            typedef natus::soil::rptr< win32_window_handle_ptr_t > win32_window_handle_rptr_t ;
        }
    }
}