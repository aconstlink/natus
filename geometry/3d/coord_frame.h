
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
        class NATUS_GEOMETRY_API coord_frame
        {
        public:

            struct input_params
            {
                natus::math::vec3f_t scale = natus::math::vec3f_t(1.0f) ;
            };
            natus_typedef( input_params ) ;

        public:

            static natus::geometry::result make( flat_line_mesh_ptr_t, input_params_cref_t ) ;
        };
        natus_typedef( coord_frame ) ;
    }
}