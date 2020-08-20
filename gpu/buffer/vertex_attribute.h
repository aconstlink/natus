#pragma once

#include <natus/ntd/string.hpp>

namespace natus
{
    namespace gpu
    {
        enum class vertex_attribute
        {
            undefined,
            position,
            normal,
            tangent,
            binormal,
            color0,
            color1,
            color2,
            color3,
            color4,
            color5,
            texcoord0,
            texcoord1,
            texcoord2,
            texcoord3,
            texcoord4,
            texcoord5,
            texcoord6,
            texcoord7
        } ;

        namespace detail
        {
            static natus::ntd::string_t const vertex_attribute_string_array[] =
            {
                "undefined", "position", "normal", "tangent", "binormal", "color0", "color1", "texcood0",
                "texcood1", "texcood2", "texcood3", "texcood4", "texcood5", "texcood6", "texcood7"
            } ;
        }

        static natus::ntd::string_cref_t to_string( vertex_attribute va ) noexcept
        {
            return detail::vertex_attribute_string_array[ size_t( va ) ] ;
        }

        /// convert a number to a texture coordinate. i must be in [0, max_texcoord]
        static natus::gpu::vertex_attribute texcoord_vertex_attribute_by( size_t const i ) noexcept
        {
            size_t const num_texcoords = size_t( vertex_attribute::texcoord7 ) -
                size_t( vertex_attribute::texcoord0 ) ;

            if( i >= num_texcoords )
                return vertex_attribute::undefined ;

            return natus::gpu::vertex_attribute(
                size_t( vertex_attribute::texcoord0 ) + i ) ;
        }
    }
}