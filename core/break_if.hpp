#pragma once

#include "types.hpp"

#include "assert.h"
#include "boolean.h"

#include <cstdlib>

namespace natus
{
    namespace core
    {
        using namespace natus::core::types ;

        static void_t break_if( bool_t condition )
        {
            natus_assert( natus::core::is_not( condition ) ) ;
        }
    }
}


