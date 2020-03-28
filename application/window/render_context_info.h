#pragma once

#include "../typedefs.h"

namespace natus
{    
    namespace application
    {
        struct render_context_info
        {
            /// RGB color bits.
            size_t color_bits = 24 ;

            /// depth bits.
            size_t depth_bits = 24 ;

            /// Specifies if double buffering should
            /// be used.
            bool_t double_buffer = true ;

            /// Allows to dis-/enable the vsync.
            bool_t vsync_enabled = true ;
        } ;
        natus_typedefs( render_context_info, render_context_info ) ;

        //static const render_context_info default_render_context_info = {24, 24, false, true, false} ;
    }
}