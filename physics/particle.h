
#pragma once

#include "api.h"
#include "typedefs.h"

#include <natus/math/vector/vector2.hpp>

namespace natus
{
    namespace physics
    {
        struct particle
        {
            float_t age = 1.0f ; // in seconds
            
            float_t mass = 1.0f ;

            natus::math::vec2f_t force ;

            natus::math::vec2f_t pos ;
            natus::math::vec2f_t vel ;
            natus::math::vec2f_t acl ;
        };
        natus_typedef( particle ) ;
    }
}
