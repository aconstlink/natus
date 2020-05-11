#pragma once

#include "../../types.h"
#include <natus/ogl/gl/gl.h>

namespace natus
{
    namespace gpu
    {
        namespace gl3
        {
            static GLenum convert( natus::gpu::primitive_type const pt ) noexcept
            {
                switch( pt )
                {
                case natus::gpu::primitive_type::lines: return GL_LINES ;
                case natus::gpu::primitive_type::triangles: return GL_TRIANGLES ;
                case natus::gpu::primitive_type::points: return GL_POINTS ;
                default:break ;
                }
                return 0 ;
            }
        }
    }
}