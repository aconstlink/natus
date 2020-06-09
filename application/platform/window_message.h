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
            int_t x ;
            int_t y ;
            size_t w ;
            size_t h ;
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
    }
}


