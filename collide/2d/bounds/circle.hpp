
#pragma once

#include "../../typedefs.h"
#include <natus/math/vector/vector2.hpp>

namespace natus
{
    namespace collide
    {
        namespace n2d
        {
            template< typename type_t >
            class circle
            {
                typedef circle< type_t > this_t ;

                typedef natus::math::vector2< type_t >  vec2_t ;
                typedef vec2_t const & vec2_cref_t ;
                typedef vec2_t & vec2_ref_t ;

            private:

                type_t _radius ;
                vec2_t _center ;

            public:

                circle( void_t ) noexcept
                {
                    _radius = type_t(0) ;
                }

                circle( vec2_cref_t center, type_t radius ) noexcept
                {
                    _radius = radius ;
                    _center = center ;
                }

                bool_t include( vec2_cref_t point ) const noexcept
                {
                    return (point - get_center()).length2() < get_radius2() ;
                }

                /// returns the squared distance to the bounds of this
                /// circle to the point p.
                type_t squared_distance_to( vec2_cref_t p ) const noexcept
                {
                    const vec2_t dif = p - _center ;
                    return dif.length2() - this_t::get_radius2() ;
                }

                /// returns the squared distance to the bounds of this
                /// circle to the point p.
                type_t distance_to( vec2_cref_t p ) const noexcept
                {
                    const vec2_t dif = p - _center ;
                    return dif.length() - this_t::get_radius() ;
                }

            public:

                type_t get_radius( void_t ) const noexcept { return _radius ; }
                type_t get_radius2( void_t ) const noexcept { return _radius * _radius ; }

                vec2_cref_t get_center( void_t ) const noexcept { return _center ; }
            };
        }
    }
}
