#pragma once

namespace natus
{
    namespace nsl
    {
        enum class language_class
        {
            unknown,
            glsl,
            hlsl
        };

        static language_class to_language_class( natus::ntd::string_cref_t s ) noexcept
        {
            if( s == "glsl" ) return language_class::glsl ;
            else if( s == "hlsl" ) return language_class::hlsl ;
            return language_class::unknown ;
        }

        enum class api_type
        {
            gl3,
            es3,
            d3d11
        };

        enum class shader_type
        {
            unknown,
            vertex_shader,
            pixel_shader
        };
        static shader_type to_shader_type( natus::ntd::string_cref_t s ) noexcept
        {
            if( s == "vertex_shader" ) return shader_type::vertex_shader ;
            else if( s == "pixel_shader" ) return shader_type::pixel_shader ;
            return shader_type::unknown ;
        }

        enum class flow_qualifier
        {
            unknown,
            in,
            out,
            global
        };
        static flow_qualifier to_flow_qualifier( natus::ntd::string_cref_t s ) noexcept
        {
            if( s == "in" ) return flow_qualifier::in ;
            else if( s == "out" ) return flow_qualifier::out ;
            else if( s == "" || s == "global" ) return flow_qualifier::global ;
            return flow_qualifier::unknown ;
        }

        static natus::ntd::string_cref_t to_string( natus::nsl::flow_qualifier const fq ) noexcept
        {
            static natus::ntd::string_t const __strings[] = { 
                "unknown", "in", "out", "global" } ;
            return __strings[ size_t( fq ) ] ;
        }

        enum class type
        {
            tfloat,
            tint,
            tuint,
            tvec1,
            tvec2,
            tvec3,
            tmat2,
            tmat3,
            tmat4,
            any_texture
        };
    }
}