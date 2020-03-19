#pragma once 

#include "types.hpp"

namespace natus
{
    namespace core
    {
        using namespace natus::core::types ;

        static bool_t is_not( bool_t const b )
        {
            return !b ;
        }

        static bool_t is( bool_t const b )
        {
            return b;
        }
    }
}

