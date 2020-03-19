#pragma once

#include "checks.h"

namespace natus
{
    namespace core
    {
        using namespace natus::core::types ;

        /// @params B base class
        /// @params T concrete class
        template< typename To, typename From >
        bool_t can_cast( From ptr )
        {
            return natus::core::is_not_nullptr( dynamic_cast< To >( ptr ) ) ;
        }
    }
}