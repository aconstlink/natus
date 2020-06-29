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
        namespace es3
        {
            namespace detail
            {
                static const GLenum convert_blend_factor_gl[] =
                {
                    GL_ZERO,
                    GL_ONE,
                    GL_SRC_COLOR,
                    GL_ONE_MINUS_SRC_COLOR,
                    GL_DST_COLOR,
                    GL_ONE_MINUS_DST_COLOR,
                    GL_SRC_ALPHA,
                    GL_ONE_MINUS_SRC_ALPHA,
                    GL_DST_ALPHA,
                    GL_ONE_MINUS_DST_ALPHA,
                    GL_CONSTANT_COLOR,
                    GL_ONE_MINUS_CONSTANT_COLOR,
                    GL_CONSTANT_ALPHA,
                    GL_ONE_MINUS_CONSTANT_ALPHA,
                    GL_SRC_ALPHA_SATURATE
                } ;

                static const size_t convert_blend_factors_gl_size =
                    sizeof( convert_blend_factor_gl ) / sizeof( convert_blend_factor_gl[ 0 ] ) ;
            }

            static GLenum convert( blend_factor const bf ) noexcept
            {
                return detail::convert_blend_factor_gl[ size_t( bf ) ] ;
            }
        }

        namespace es3
        {
            static GLenum convert_to_gl_format( natus::gpu::image_format const imf, 
                natus::gpu::image_element_type const t )
            {

                switch( imf )
                {
                case image_format::depth:
                    return GL_DEPTH_COMPONENT ;

                case image_format::rgb:
                    switch( t )
                    {
                    case natus::gpu::image_element_type::uint8: return GL_RGB ;
                    case natus::gpu::image_element_type::int16: return GL_RGB16I ;
                    case natus::gpu::image_element_type::int32: return GL_RGB32I ;
                    case natus::gpu::image_element_type::uint16: return GL_RGB16UI ;
                    case natus::gpu::image_element_type::uint32: return GL_RGB32UI ;
                    case natus::gpu::image_element_type::float16: return GL_RGB16F ;
                    case natus::gpu::image_element_type::float32: return GL_RGB32F ;
                    default: break ;
                    }
                    break  ;

                case image_format::rgba:
                    switch( t )
                    {
                    case natus::gpu::image_element_type::uint8: return GL_RGBA ;
                    case natus::gpu::image_element_type::int16: return GL_RGBA16I ;
                    case natus::gpu::image_element_type::int32: return GL_RGBA32I ;
                    case natus::gpu::image_element_type::uint16: return GL_RGBA16UI ;
                    case natus::gpu::image_element_type::uint32: return GL_RGBA32UI ;
                    case natus::gpu::image_element_type::float16: return GL_RGBA16F ;
                    case natus::gpu::image_element_type::float32: return GL_RGBA32F ;
                    default: break ;
                    }
                    break  ;

                case image_format::intensity:
                    switch( t )
                    {
                    case natus::gpu::image_element_type::uint8: return GL_RED ;
                    case natus::gpu::image_element_type::int16: return GL_R16I ;
                    case natus::gpu::image_element_type::int32: return GL_R32I ;
                    case natus::gpu::image_element_type::uint16: return GL_R16UI ;
                    case natus::gpu::image_element_type::uint32: return GL_R32UI ;
                    case natus::gpu::image_element_type::float16: return GL_R16F ;
                    case natus::gpu::image_element_type::float32: return GL_R32F ;
                    default: break ;
                    }
                    break  ;

                default: break ;

                }
                return 0 ;
            }

            static GLenum convert_to_gl_pixel_format( natus::gpu::image_format const imf )
            {
                switch( imf )
                {
                case natus::gpu::image_format::depth: return GL_DEPTH_COMPONENT ;
                case natus::gpu::image_format::rgb: return GL_RGB ;
                case natus::gpu::image_format::rgba: return GL_RGBA ;
                case natus::gpu::image_format::intensity: return GL_RED ;
                default: break ;
                }
                return 0 ;
            }

            static GLenum convert_to_gl_pixel_type( natus::gpu::image_element_type const pt )
            {
                switch( pt )
                {
                case natus::gpu::image_element_type::uint8: return GL_UNSIGNED_BYTE ;
                case natus::gpu::image_element_type::uint16: return GL_UNSIGNED_SHORT ;
                case natus::gpu::image_element_type::uint32: return GL_UNSIGNED_INT ;

                case natus::gpu::image_element_type::int8: return GL_BYTE ;
                case natus::gpu::image_element_type::int16: return GL_SHORT ;
                case natus::gpu::image_element_type::int32: return GL_INT ;
                
                case natus::gpu::image_element_type::float16: return GL_FLOAT ;
                case natus::gpu::image_element_type::float32: return GL_FLOAT ;
                }
                return 0 ;
            }
        }

        namespace es3
        {
            namespace detail
            {
                static const GLenum filter_modes_gl[] = {
                    GL_TEXTURE_MAG_FILTER,
                    GL_TEXTURE_MIN_FILTER
                } ;

                static const GLenum filter_types_gl[] = {
                    GL_NEAREST,
                    GL_LINEAR
                } ;
            }

            static GLenum convert( texture_filter_mode const m ) noexcept
            {
                return natus::gpu::es3::detail::filter_modes_gl[ ( size_t ) m ] ;
            }

            static GLenum convert( texture_filter_type const t ) noexcept
            {
                return natus::gpu::es3::detail::filter_types_gl[ ( size_t ) t ] ;
            }

            namespace detail
            {
                static const GLenum wrap_modes_gl[] = {
                    GL_TEXTURE_WRAP_S,
                    GL_TEXTURE_WRAP_T,
                    GL_TEXTURE_WRAP_R
                } ;

                static const GLenum wrap_types_gl[] = {
                    GL_CLAMP_TO_EDGE,
                    GL_REPEAT,
                    GL_CLAMP_TO_EDGE,
                    GL_CLAMP_TO_EDGE,
                    GL_MIRRORED_REPEAT
                } ;
            }

            static GLenum convert( texture_wrap_mode const m ) noexcept {
                return natus::gpu::es3::detail::wrap_modes_gl[ ( size_t ) m ] ;
            }

            static GLenum convert( texture_wrap_type const t ) noexcept {
                return natus::gpu::es3::detail::wrap_types_gl[ ( size_t ) t ] ;
            }
        }
    }
}
