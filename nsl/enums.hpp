#pragma once

namespace natus
{
    namespace nsl
    {
        enum class api_type
        {
            gl3,
            es3
        };

        enum class shader_type
        {
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