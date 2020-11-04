
#pragma once

#include "../../typedefs.h"
#include "../../api.h"


#include <natus/ntd/vector.hpp>
#include <functional>

namespace natus
{
    namespace geometry
    {
        struct NATUS_GEOMETRY_API fractal_3d
        {
            struct fractal_object
            {
                natus::math::vec3f_t pos ;
                float_t side_width ;
            };
            natus_typedef( fractal_object ) ;

            natus_this_typedefs( fractal_3d ) ;

            natus_typedefs( natus::ntd::vector<fractal_object>, objects ) ;

            typedef std::function< objects_t ( fractal_object_cref_t ) > divider_funk_t ;
            

            static objects_t menger_sponge( fractal_object_cref_t ) ;
            static objects_t sierpinski_tetra( fractal_object_cref_t ) ;

            static divider_funk_t menger_sponge_funk( void_t ) ;
            static divider_funk_t sierpinski_tetra_funk( void_t ) ;

            static objects_t iterate( divider_funk_t, fractal_object_cref_t, size_t ) ;
        };
    }
}
