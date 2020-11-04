
#pragma once

#include "../typedefs.h"

#include <natus/math/vector/vector2.hpp>

namespace natus
{
    namespace geometry
    {
        struct helper_2d
        {        
            /// @param index the vector index
            static void_t vector_to_array( size_t index, natus::math::vec2f_cref_t in_vec, floats_ref_t to ) 
            {
                to[index+0] = in_vec.x() ;
                to[index+1] = in_vec.y() ;
            }
        };
    }
}
