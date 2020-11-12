
#pragma once

#include <natus/ntd/string.hpp>

namespace natus
{
    namespace graphics
    {
        enum class binding_point
        {
            undefined,
            object_matrix,
            world_matrix,
            projection_matrix,  // mat4()
            view_matrix,        // mat4()
            camera_matrix,      // mat3()
            camera_position,    // vec3( x, y, z )
            viewport_dimension  // vec4( x, y, w, h )
        };

        namespace detail
        {
            static natus::ntd::string_t const __binding_point_names[] = { 
                "undefined", "projection_matrix","view_matrix","camera_matrix","camera_position",
                "viewport_dimension"
            } ;
        }

        static natus::ntd::string_cref_t to_string( binding_point const bp ) noexcept
        {
            return detail::__binding_point_names[size_t(bp)] ;
        }
    }
}