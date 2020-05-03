
#pragma once

#include <natus/std/string.hpp>

namespace natus
{
    namespace gpu
    {
        enum class binding_point
        {
            undefined,
            projection_matrix,
            view_matrix,
            camera_matrix,
            camera_position
        };

        namespace detail
        {
            static natus::std::string_t const __binding_point_names[] = { 
                "undefined", "projection_matrix","view_matrix","camera_matrix","camera_position"
            } ;
        }

        static natus::std::string_cref_t to_string( binding_point const bp ) noexcept
        {
            return detail::__binding_point_names[size_t(bp)] ;
        }
    }
}