
#pragma once

#include <natus/std/string.hpp>

namespace natus
{
    namespace gpu
    {
        enum class binding_point
        {
            undefined,
            projection_matrix,  // mat4()
            view_matrix,        // mat4()
            camera_matrix,      // mat3()
            camera_position,    // vec3( x, y, z )
            viewport_dimension  // vec4( x, y, w, h )
        };

        namespace detail
        {
            static natus::std::string_t const __binding_point_names[] = { 
                "undefined", "projection_matrix","view_matrix","camera_matrix","camera_position",
                "viewport_dimension"
            } ;
        }

        static natus::std::string_cref_t to_string( binding_point const bp ) noexcept
        {
            return detail::__binding_point_names[size_t(bp)] ;
        }
    }
}