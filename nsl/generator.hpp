
#pragma once

#include "typedefs.h"
#include "enums.hpp"
#include "symbol.hpp"

#include <natus/ntd/vector.hpp>

namespace natus
{
    namespace nsl
    {
        struct generated_code
        {
            struct code 
            {
                natus::nsl::shader_type st ;
                natus::ntd::string_t shader ;
            };
            natus_typedef( code ) ;
            natus::ntd::vector< code > codes ;
        };
        natus_typedef( generated_code ) ;

        struct generateable
        {
            // variables
            // shaders
            // config
        };
        natus_typedef( generateable ) ;
    }
}