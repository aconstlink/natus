
#pragma once

#include "api.h"
#include "typedefs.h"
#include "protos.h"



namespace natus
{
    namespace application
    {
        class NATUS_APPLICATION_API global
        {
            natus_this_typedefs( global ) ;

        public:

            static iwindow_rptr_t create_window( void_t ) noexcept ;
            static iwindow_rptr_t create_render_window( void_t ) noexcept ;

        };
        natus_typedef( global ) ;
    }
}