#pragma once

#include "egl_context.h"
#include "../xlib/xlib_window.h"

#include "../platform_window.h"

namespace natus
{
    namespace application
    {
        namespace egl
        {
            class window : public platform_window
            {
                natus_this_typedefs( window ) ;

            private:

                context_res_t _context ;
                xlib::window_res_t _window ;

                bool_t _vsync = true ;

            public:
 
                window( void_t ) ;
                window( gl_info_cref_t gli, window_info_cref_t wi ) ;
                window( this_rref_t ) ;
                virtual ~window( void_t ) ;

            public:

                context_res_t get_context( void_t ) const { return _context ; }
                xlib::window_res_t get_window( void_t ) { return _window ; }

            private:

                xlib::window_res_t create_window( gl_info_in_t, window_info_in_t ) ;
                EGLNativeWindowType create_egl_window( window_info_in_t ) ;
            };
            natus_typedef( window ) ;
            typedef natus::soil::res< window_t > window_res_t ;
        }
    }
}
