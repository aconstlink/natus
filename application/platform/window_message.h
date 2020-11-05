#pragma once

#include "../typedefs.h"

namespace natus
{
    namespace application
    {
        struct show_message
        {
            bool_t show ;
        };
        natus_typedef( show_message ) ;

        struct resize_message
        {
            bool_t position = false ;
            int_t x = 0 ;
            int_t y = 0 ;

            bool_t resize = false ;
            size_t w = 0 ;
            size_t h = 0 ;

            resize_message & combine( resize_message const & rhv ) noexcept
            {
                if( rhv.position )
                {
                    position = true ;
                    x = rhv.x ;
                    y = rhv.y ;
                }
                if( rhv.resize )
                {
                    resize = true ;
                    w = rhv.w ;
                    h = rhv.h ;
                }
                return *this ;
            }
        };
        natus_typedef( resize_message ) ;

        struct close_message
        {
            bool_t close ;
        };
        natus_typedef( close_message ) ;

        struct screen_dpi_message
        {
            uint_t dpi_x ;
            uint_t dpi_y ;
        };
        natus_typedef( screen_dpi_message ) ;

        struct screen_size_message
        {
            uint_t width ;
            uint_t height ;
        };
        natus_typedef( screen_size_message ) ;

        struct vsync_message
        {
            bool_t on_off ;
        };
        natus_typedef( vsync_message ) ;

        struct fullscreen_message
        {
            bool_t on_off ;
        };
        natus_typedef( fullscreen_message ) ;
    }
}


