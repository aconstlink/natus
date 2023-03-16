#pragma once

#include "../../vector/vector2.hpp"

namespace natus
{
    namespace math
    {
        namespace m2d
        {
            template< typename type_t >
            class ray
            {
                natus_this_typedefs( ray< type_t > ) ;
                natus_typedefs( natus::math::vector2< type_t >, vec2 ) ;

            private:

                vec2_t _origin ;
                vec2_t _dir ;

            public:

                ray( void_t ) noexcept {}

                ray( vec2_cref_t orig, vec2_cref_t dir ) noexcept 
                {
                    _origin = orig ;
                    _dir = dir ;
                }

                vec2_cref_t get_origin( void_t ) const noexcept { return _origin ; }
                vec2_cref_t get_direction( void_t ) const noexcept { return _dir ; }

                vec2_t point_at( type_t dist ) const noexcept { return _origin + _dir * dist ; }

                this_ref_t translate_to( this_t::vec2_cref_t p ) noexcept 
                {
                    _origin = p ;
                    return *this ;
                }

                void_t set_direction( vec2_cref_t d ) noexcept { _dir = d ; }
            };
        }
        typedef m2d::ray< float_t > ray2f_t ;
    }
}