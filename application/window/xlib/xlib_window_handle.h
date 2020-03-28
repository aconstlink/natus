//------------------------------------------------------------
// snakeoil (c) Alexis Constantin Link
// Distributed under the MIT license
//------------------------------------------------------------
#ifndef _SNAKEOIL_APPLICATION_WINDOW_XLIB_XLIB_WINDOW_HANDLE_H_
#define _SNAKEOIL_APPLICATION_WINDOW_XLIB_XLIB_WINDOW_HANDLE_H_

#include "../iwindow_handle.h"

#include <X11/Xlib.h>

namespace so_app
{
    namespace so_xlib
    {
        class xlib_window_handle : public iwindow_handle
        {
            so_this_typedefs( xlib_window_handle ) ;

        private:

            Display * _display ;
            Window _handle ;

        public:

            xlib_window_handle( void_t ) ;
            xlib_window_handle( Display *, Window wnd ) ;
            xlib_window_handle( this_rref_t rhv ) ;
            virtual ~xlib_window_handle( void_t ) ;

        public:

            this_ref_t operator = ( this_rref_t ) ;

        public:

            Window set_handle( Window ) ;
            Window get_handle( void_t ) ;
            Display* get_display( void_t ) ;
            bool_t is_valid( void_t ) ;
        };
        so_typedefs( xlib_window_handle, xlib_window_handle ) ;
    }
}

#endif

