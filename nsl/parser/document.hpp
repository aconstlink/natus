
#pragma once

#include "../typedefs.h"

#include <natus/ntd/vector.hpp>

namespace natus
{
    namespace nsl
    {
        namespace parse
        {
            struct config
            {
            };

            struct library
            {
                struct shader
                {
                    natus::ntd::vector< natus::ntd::string_t > versions ;
                    natus::ntd::vector< natus::ntd::string_t > fragments ;
                };
                natus::ntd::vector< shader > shaders ;
                
                natus::ntd::vector< natus::ntd::string_t > variables ;
                natus::ntd::vector< natus::ntd::string_t > names ;
            };
            natus_typedef( library ) ;
            natus_typedefs( natus::ntd::vector< library_t >, libraries ) ;
        }
    }
}