#pragma once

#include "api.h"
#include "typedefs.h"

#include <natus/std/vector.hpp>

namespace natus
{
    namespace system
    {
        class NATUS_SYSTEM_API global
        {
            natus_this_typedefs( global ) ;

        public: 

            static natus::system::os_type compiled_os_type( void_t ) ;
            static natus::std::vector< natus::system::window_type > compiled_window_types( void_t ) ;
            static natus::std::vector< natus::system::graphics_type > compiled_graphics_types( void_t ) ;
            
            static bool_t has_compiled_window_type( natus::system::window_type const ) ;
            static bool_t has_compiled_graphics_type( natus::system::graphics_type const ) ;
        };
        natus_typedef( global ) ;
    }
}