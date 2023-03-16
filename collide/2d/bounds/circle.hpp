
#pragma once

#include "../../typedefs.h"
#include <natus/math/vector/vector2.hpp>
#include <natus/math/vector/vector3.hpp>

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

                natus_typedefs( natus::math::vector2< type_t >, vec2 ) ;
                natus_typedefs( natus::math::vector3< type_t >, vec3 ) ;

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

                /// returns the distance to the bounds of this
                /// circle to the point p.
                type_t distance_to( vec2_cref_t p ) const noexcept
                {
                    const vec2_t dif = p - _center ;
                    return dif.length() - this_t::get_radius() ;
                }

                // creates a normal vector in the direction to the point p
                vec2_t calculate_normal_for( vec2_cref_t p ) const noexcept
                {
                    return (p - _center).normalized() ;
                }

                // create a normal vector in the direction to the point p 
                // with the distance to its own border(the radius) in the z component.
                vec3_t calculate_normal_distance_to_bound_for( vec2_cref_t p ) const noexcept
                {
                    return vec3_t( this_t::calculate_normal_for( p ), _radius ) ;
                }

            public:

                type_t get_radius( void_t ) const noexcept { return _radius ; }
                type_t get_radius2( void_t ) const noexcept { return _radius * _radius ; }

                vec2_cref_t get_center( void_t ) const noexcept { return _center ; }
                void_t set_center( vec2_cref_t p ) noexcept { _center = p ; }
                void_t set_radius( float_t const r ) noexcept { _radius = r ; }
            };
            natus_typedefs( circle< float_t >, circlef ) ;
        }
    }
}
