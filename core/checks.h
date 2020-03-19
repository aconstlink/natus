#pragma once

#include "types.hpp"

namespace natus
{
    namespace core
    {
        using namespace natus::core::types ;

        template< typename T >
        static bool_t is_nullptr( T const* const ptr )
        {
            return ptr == nullptr ;
        }

        template< typename T >
        static bool_t is_not_nullptr( T const* const ptr )
        {
            return ptr != nullptr ;
        }
    }
}