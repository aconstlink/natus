#pragma once

#include "wgl_context.h"
#include "../win32/win32_window.h"

#include "../platform_window.h"

namespace natus
{
    namespace application
    {
        namespace wgl
        {
            class NATUS_APPLICATION_API window : public platform_window
            {
                natus_this_typedefs( window ) ;

            private:

                context_res_t _context ;
                win32::window_res_t _window ;

                bool_t _vsync = true ;

            public:

                window( void_t ) ;
                window( gl_info_cref_t gli, window_info_cref_t wi ) ;
                window( this_rref_t ) ;
                virtual ~window( void_t ) ;

            public:

                context_res_t get_context( void_t ) const { return _context ; }
                win32::window_res_t get_window( void_t ) { return _window ; }
            };
            natus_typedef( window ) ;
            typedef natus::soil::res< window_t > window_res_t ;
        }
    }
}