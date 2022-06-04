#pragma once

#include "angle.hpp"
#include "constants.hpp"

namespace natus
{    
    namespace math
    {
        template< typename T >
        class degree
        {
            natus_this_typedefs( degree<T> ) ;
            natus_typedefs( T, type ) ;

            natus_typedefs( angle<T>, angle ) ;

        public:

            static type_t val_to_radian( type_t const degree )
            {
                return degree * natus::math::constants<type_t>::pi() / type_t( 180 ) ;
            }

            static angle_t to_radian( type_t const deg )
            {
                return angle_t( this_t::val_to_radian( deg ) ) ;
            }

        };
    }
}