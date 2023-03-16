#pragma once

#include "../typedefs.h"
#include "../hit_test_types.h"

#include "bounds/aabb.hpp"
#include "bounds/circle.hpp"

#include <natus/math/primitive/2d/ray.hpp>
#include <natus/math/vector/vector4.hpp>
#include <natus/math/vector/vector4b.hpp>

namespace natus 
{
    namespace collide
    {
        namespace n2d
        {
            template< typename T >
            struct hit_tests
            {
                typedef T type_t ;
                natus_this_typedefs( hit_tests< T > ) ;

                natus_typedefs( natus::math::m2d::ray< T >, ray ) ;
                natus_typedefs( aabb< type_t >, aabb ) ;
                natus_typedefs( circle< type_t >, circle ) ;
                natus_typedefs( natus::math::vector2< type_t >, vec2 ) ;
                natus_typedefs( natus::math::vector4< type_t >, vec4 ) ;

                /// @return true, if circle and aabb overlap, otherwise false.
                static bool_t aabb_circle_overlap( aabb_cref_t bound_aabb, circle_cref_t bound_circle ) noexcept
                {
                    const type_t d = bound_aabb.squared_distance_to( bound_circle.get_center() ) ;
                    const type_t r = bound_circle.get_radius2() ;

                    return d <= r ;
                }

                /// @return true, if circle and aabb overlap, otherwise false.
                /// @return closest point to circle on intersection. Invalid if function returns false.
                static bool_t aabb_circle_overlap( aabb_cref_t bound_aabb, circle_cref_t bound_circle, vec2_out_t closest_point ) noexcept
                {
                    bound_aabb.closest_point_to( bound_circle.get_center(), closest_point ) ;
                    return (closest_point - bound_circle.get_center()).length2()  <= bound_circle.get_radius2() ;
                }

                /// box against circle
                /// @return 
                /// outside: if box and sphere are completely outside of each other
                /// inside: the box is completely inside of the sphere
                /// intersect: both box' and sphere's surface intersect
                static hit_test_type aabb_circle( aabb_cref_t volume_a, circle_cref_t volume_b ) noexcept
                {
                    const vec2_t max = volume_b.get_center() - volume_a.get_max() ;
                    const vec2_t min = volume_b.get_center() - volume_a.get_min() ;
                    const type_t radius = volume_b.get_radius() ;

                    // test full outside
                    {
                        auto const minb = min.less_than( vec2_t( -radius ) ) ;
                        auto const maxb = max.greater_than( vec2_t( radius ) ) ;

                        if( minb.any() || maxb.any() ) return hit_test_type::outside ;
                    }

                    // test intersection
                    {
                        auto const minb = min.greater_than( vec2_t( -radius ) ) && min.less_than( vec2_t( radius ) ) ;
                        auto const maxb = max.greater_than( vec2_t( -radius ) ) && max.less_than( vec2_t( radius ) ) ;

                        if( minb.any() || maxb.any() ) return hit_test_type::intersect ;
                    }

                    return hit_test_type::inside ;
                }

                // circle against circle
                static hit_test_type circle_circle_overlap( circle_cref_t a, circle_cref_t b ) noexcept
                {
                    auto const dv = a.get_center() - b.get_center() ;
                    auto const f = (a.get_radius() + b.get_radius()) ;
                    return ( dv.dot( dv ) > f*f ) ? hit_test_type::outside : hit_test_type::overlap ;
                }

                static hit_test_type ray_circle( ray_cref_t r, circle_cref_t c ) noexcept
                {
                    vec2_t d ;
                    return this_t::ray_circle( r, c, d ) ;
                }

                // solving t = -b +- sqrt( b*b - c ) where b*b - c is the discr(minant)
                static hit_test_type ray_circle( ray_cref_t ray, circle_cref_t circ, vec2_out_t ds ) noexcept
                {
                    auto const m = ray.get_origin() - circ.get_center() ;
                    float_t const b = m.dot( ray.get_direction() ) ;
                    float_t const c = m.dot( m ) - circ.get_radius2() ;

                    // ray and circle disjoint(c >0 ) and ray pointing away (b>0)
                    if( c > 0.0f && b > 0.0f ) return hit_test_type::outside ;

                    float_t const discr = b*b - c ;

                    // no real solution, no intersection
                    if( discr < 0.0f ) return hit_test_type::outside ;

                    float_t s = std::sqrt( discr ) ;

                    float_t const x = -b - s ;
                    float_t const y = -b + s ;
                    
                    ds = x > 0.0f ? vec2_t( x, y ) : vec2_t( y, x ) ;

                    return ds.less_than( 0.0f ).any() ? hit_test_type::inside : hit_test_type::intersect ;
                }

                static hit_test_type ray_aabb( ray_cref_t r, aabb_cref_t b ) noexcept
                {
                    return hit_test_type::outside ;
                }
            };
        }
    }
}