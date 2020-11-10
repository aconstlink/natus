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
            es3
        };

        enum class shader_type
        {
            unknown,
            vertex_shader,
            pixel_shader
        };

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