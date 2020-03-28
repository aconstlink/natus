#pragma once

#include "../../typedefs.h"

namespace natus
{
    namespace application
    {
        namespace win32
        {
            /// @todo can we delete it. it should have been replaced
            /// by the window_info sucture.
            struct win32_window_info
            {
                int_t x = 0 ;
                int_t y = 0 ;
                int_t w = 100 ;
                int_t h = 100 ;

                bool_t fullscreen = false ;
                bool_t show_cursor = true ;
                std::string window_name = std::string( "window name" );

            };
            natus_typedefs( win32_window_info, win32_window_info ) ;
        }
    }
}
