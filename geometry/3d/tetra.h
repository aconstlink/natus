
#pragma once

#include "../result.h"
#include "../typedefs.h"
#include "../protos.h"
#include "../api.h"

#include <natus/math/vector/vector2.hpp>
#include <natus/math/vector/vector3.hpp>

namespace natus
{
    namespace geometry
    {
        struct NATUS_GEOMETRY_API tetra
        {
            struct input_params
            {
                /// per dimension scale in object space!
                natus::math::vec3f_t scale = natus::math::vec3f_t( 1.0f );
                /// per dimension rotation - euler angles in object space!
                natus::math::vec3f_t euler = natus::math::vec3f_t( 0.0f );
            };
            natus_typedef( input_params ) ;

            static natus::geometry::result make( polygon_mesh_ptr_t, input_params_cref_t ) ;
            
        };
    }
}
