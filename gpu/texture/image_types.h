

#pragma once

namespace natus
{
    namespace gpu
    {
        enum class image_format
        {
            rgb,
            rgba,
            depth,
            stencil
        };

        // per channel type and bit-ness
        enum class image_element_type
        {
            float8,
            float16,
            float32,
            int8,
            int16,
            int32,
            uint8,
            uint16,
            uint32
        };

        // returns the number of channels.
        static size_t size_of( natus::gpu::image_format const imf ) 
        {
            switch( imf )
            {
            case natus::gpu::image_format::rgb: return 3 ;
            case natus::gpu::image_format::rgba: return 4 ;
            default: break ;
            }
            return 0 ;
        }

        // returns the sib of a type
        static size_t size_of( natus::gpu::image_element_type const iet )
        {
            switch( iet )
            {
            case natus::gpu::image_element_type::float8: return 1 ;
            case natus::gpu::image_element_type::float16: return 2 ;
            case natus::gpu::image_element_type::float32: return 4 ;

            case natus::gpu::image_element_type::int8: return 1 ;
            case natus::gpu::image_element_type::int16: return 2 ;
            case natus::gpu::image_element_type::int32: return 4 ;

            case natus::gpu::image_element_type::uint8: return 1 ;
            case natus::gpu::image_element_type::uint16: return 2 ;
            case natus::gpu::image_element_type::uint32: return 4 ;

            default: break ;
            }
            return 0 ;
        }
    }
}