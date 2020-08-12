#pragma once

#include "glx_context.h"
#include "../xlib/xlib_window.h"

#include "../platform_window.h"

namespace natus
{
    namespace application
    {
        namespace glx
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

                static GLXFBConfig get_config( void_t ) ;

            public:

                context_res_t get_context( void_t ) const { return _context ; }
                xlib::window_res_t get_window( void_t ) { return _window ; }

            private:

                Window create_glx_window( window_info_in_t ) ;
            };
            natus_typedef( window ) ;
            typedef natus::memory::res< window_t > window_res_t ;
        }
    }
}
