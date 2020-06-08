#pragma once

#include "../../types.h"
#include <natus/ogl/es/es.h>

namespace natus
{
    namespace gpu
    {
        namespace es3
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

            static ::std::pair< natus::gpu::type, natus::gpu::type_struct >  
                to_type_type_struct( GLenum const e ) noexcept
            {
                switch( e )
                {
                case GL_FLOAT: 
                    return ::std::make_pair( natus::gpu::type::tfloat, natus::gpu::type_struct::vec1 ) ;
                case GL_FLOAT_VEC2: 
                    return ::std::make_pair( natus::gpu::type::tfloat, natus::gpu::type_struct::vec2 ) ;
                case GL_FLOAT_VEC3: 
                    return ::std::make_pair( natus::gpu::type::tfloat, natus::gpu::type_struct::vec3 ) ;
                case GL_FLOAT_VEC4:
                    return ::std::make_pair( natus::gpu::type::tfloat, natus::gpu::type_struct::vec4 ) ;
                case GL_INT:
                    return ::std::make_pair( natus::gpu::type::tint, natus::gpu::type_struct::vec1 ) ;
                case GL_INT_VEC2:
                    return ::std::make_pair( natus::gpu::type::tint, natus::gpu::type_struct::vec2 ) ;
                case GL_INT_VEC3:
                    return ::std::make_pair( natus::gpu::type::tint, natus::gpu::type_struct::vec3 ) ;
                case GL_INT_VEC4:
                    return ::std::make_pair( natus::gpu::type::tint, natus::gpu::type_struct::vec4 ) ;
                case GL_UNSIGNED_INT:
                    return ::std::make_pair( natus::gpu::type::tuint, natus::gpu::type_struct::vec1 ) ;
                case GL_UNSIGNED_INT_VEC2:
                    return ::std::make_pair( natus::gpu::type::tuint, natus::gpu::type_struct::vec2 ) ;
                case GL_UNSIGNED_INT_VEC3:
                    return ::std::make_pair( natus::gpu::type::tuint, natus::gpu::type_struct::vec3 ) ;
                case GL_UNSIGNED_INT_VEC4:
                    return ::std::make_pair( natus::gpu::type::tuint, natus::gpu::type_struct::vec4 ) ;
                case GL_BOOL:
                    return ::std::make_pair( natus::gpu::type::tbool, natus::gpu::type_struct::vec1 ) ;
                case GL_BOOL_VEC2:
                    return ::std::make_pair( natus::gpu::type::tbool, natus::gpu::type_struct::vec2 ) ;
                case GL_BOOL_VEC3:
                    return ::std::make_pair( natus::gpu::type::tbool, natus::gpu::type_struct::vec3 ) ;
                case GL_BOOL_VEC4:
                    return ::std::make_pair( natus::gpu::type::tbool, natus::gpu::type_struct::vec4 ) ;
                case GL_FLOAT_MAT2:
                    return ::std::make_pair( natus::gpu::type::tfloat, natus::gpu::type_struct::mat2 ) ;
                case GL_FLOAT_MAT3:
                    return ::std::make_pair( natus::gpu::type::tfloat, natus::gpu::type_struct::mat3 ) ;
                case GL_FLOAT_MAT4:
                    return ::std::make_pair( natus::gpu::type::tfloat, natus::gpu::type_struct::mat4 ) ;
                case GL_SAMPLER_2D:
                case GL_SAMPLER_3D:
                case GL_INT_SAMPLER_2D:
                case GL_INT_SAMPLER_3D:
                case GL_UNSIGNED_INT_SAMPLER_2D:
                case GL_UNSIGNED_INT_SAMPLER_3D:
                case GL_SAMPLER_CUBE:
                case GL_SAMPLER_2D_SHADOW:
                case GL_SAMPLER_2D_ARRAY:
                case GL_INT_SAMPLER_2D_ARRAY:
                case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY: 
                    return ::std::make_pair( natus::gpu::type::tint, natus::gpu::type_struct::vec1 ) ;
                default: break ;
                }
                return ::std::make_pair( natus::gpu::type::undefined, natus::gpu::type_struct::undefined ) ;
            }
        }
    }
}
