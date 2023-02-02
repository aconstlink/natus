#pragma once

#include <natus/ntd/string.hpp>

namespace natus
{
    namespace graphics
    {
        enum class vertex_attribute
        {
            undefined,
            position,
            normal,
            tangent,
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
            texcoord7,
            num_attributes
        } ;

        namespace detail
        {
            static natus::ntd::string_t const vertex_attribute_string_array[] =
            {
                "undefined", "position", "normal", "tangent", "color0", "color1", "color2",
                "color3", "color4", "color5", "texcood0",
                "texcood1", "texcood2", "texcood3", "texcood4", "texcood5", "texcood6", "texcood7"
            } ;
        }

        static natus::ntd::string_cref_t to_string( vertex_attribute va ) noexcept
        {
            return detail::vertex_attribute_string_array[ size_t( va ) ] ;
        }

        /// convert a number to a texture coordinate. i must be in [0, max_texcoord]
        static natus::graphics::vertex_attribute texcoord_vertex_attribute_by( size_t const i ) noexcept
        {
            size_t const num_texcoords = size_t( vertex_attribute::texcoord7 ) -
                size_t( vertex_attribute::texcoord0 ) ;

            if( i >= num_texcoords )
                return vertex_attribute::undefined ;

            return natus::graphics::vertex_attribute(
                size_t( vertex_attribute::texcoord0 ) + i ) ;
        }

        static natus::graphics::ctype deduce_from( natus::graphics::vertex_attribute const va ) noexcept
        {
            switch( va ) 
            {
            case natus::graphics::vertex_attribute::undefined: return { natus::graphics::type::undefined, natus::graphics::type_struct::undefined } ;
            case natus::graphics::vertex_attribute::position: return { natus::graphics::type::tfloat, natus::graphics::type_struct::vec4 } ;
            case natus::graphics::vertex_attribute::normal: return { natus::graphics::type::tfloat, natus::graphics::type_struct::vec3 } ;
            case natus::graphics::vertex_attribute::tangent: return { natus::graphics::type::tfloat, natus::graphics::type_struct::vec3 } ;
            case natus::graphics::vertex_attribute::color0: return { natus::graphics::type::tfloat, natus::graphics::type_struct::vec4 } ;
            case natus::graphics::vertex_attribute::color1: return { natus::graphics::type::tfloat, natus::graphics::type_struct::vec4 } ;
            case natus::graphics::vertex_attribute::color2: return { natus::graphics::type::tfloat, natus::graphics::type_struct::vec4 } ;
            case natus::graphics::vertex_attribute::color3: return { natus::graphics::type::tfloat, natus::graphics::type_struct::vec4 } ;
            case natus::graphics::vertex_attribute::color4: return { natus::graphics::type::tfloat, natus::graphics::type_struct::vec4 } ;
            case natus::graphics::vertex_attribute::color5: return { natus::graphics::type::tfloat, natus::graphics::type_struct::vec4 } ;
            case natus::graphics::vertex_attribute::texcoord0: return { natus::graphics::type::tfloat, natus::graphics::type_struct::vec2 } ;
            case natus::graphics::vertex_attribute::texcoord1: return { natus::graphics::type::tfloat, natus::graphics::type_struct::vec2 } ;
            case natus::graphics::vertex_attribute::texcoord2: return { natus::graphics::type::tfloat, natus::graphics::type_struct::vec2 } ;
            case natus::graphics::vertex_attribute::texcoord3: return { natus::graphics::type::tfloat, natus::graphics::type_struct::vec2 } ;
            case natus::graphics::vertex_attribute::texcoord4: return { natus::graphics::type::tfloat, natus::graphics::type_struct::vec2 } ;
            case natus::graphics::vertex_attribute::texcoord5: return { natus::graphics::type::tfloat, natus::graphics::type_struct::vec2 } ;
            case natus::graphics::vertex_attribute::texcoord6: return { natus::graphics::type::tfloat, natus::graphics::type_struct::vec2 } ;
            case natus::graphics::vertex_attribute::texcoord7: return { natus::graphics::type::tfloat, natus::graphics::type_struct::vec2 } ;
                default: break ;
            }
            return { natus::graphics::type::undefined, natus::graphics::type_struct::undefined } ;
        }
    }
}
