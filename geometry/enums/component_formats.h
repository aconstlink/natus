
#pragma once

namespace natus
{
    namespace geometry
    {
        enum class vector_component_format
        {
            invalid,
            xy,
            xyz,
            xyzw
        };

        static size_t vector_component_format_to_number( vector_component_format vcf )
        {
            switch( vcf )
            {
            case vector_component_format::xy:
                return 2 ;
            case vector_component_format::xyz:
                return 3 ;
            case vector_component_format::xyzw:
                return 4 ;
            default:
                return 0 ;
            }
            return 0 ;
        }

        enum class texcoord_component_format
        {
            invalid,
            uv,
            uvw
        };

        static size_t texcoords_component_format_to_number( texcoord_component_format f )
        {
            switch( f )
            {
            case texcoord_component_format::uv:
                return 2 ;
            case texcoord_component_format::uvw:
                return 3 ;
            default:
                return 0 ;
            }
            return 0 ;
        }

        enum class image_component_format
        {
            invalid,
            rgb,
            rgba
        };
    }
}
