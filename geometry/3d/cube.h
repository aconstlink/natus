
#pragma once

#include "../result.h"
#include "../typedefs.h"
#include "../protos.h"
#include "../api.h"

#include <natus/math/vector/vector3.hpp>

namespace natus
{
    namespace geometry
    {
        struct NATUS_GEOMETRY_API cube
        {
            struct input_params
            {
                /// per dimension scale in object space!
                natus::math::vec3f_t scale = natus::math::vec3f_t(1.0f);
                /// per dimension rotation - euler angles in object space!
                natus::math::vec3f_t euler = natus::math::vec3f_t(0.0f);

                /// tessellation :
                /// vertices per side (vps) = tess + 1
                /// vertices per face (vpf) = vps * vps 
                /// vertices per cube (vpc) = vpf * 6 
                size_t tess = 1 ;
            };
            natus_typedef( input_params ) ;

            static natus::geometry::result make( polygon_mesh_ptr_t, input_params_cref_t ) ;
            static natus::geometry::result make( tri_mesh_ptr_t, input_params_cref_t ) ;
            static natus::geometry::result make( flat_line_mesh_ptr_t, input_params_cref_t ) ;
        };
        natus_typedef( cube ) ;
    }
}