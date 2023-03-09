#pragma once

#include "../../typedefs.h"
#include "../../hit_test_types.h"

#include "../bounds/aabb.hpp"
#include "../bounds/circle.hpp"

#include <natus/math/vector/vector4.hpp>
#include <natus/math/vector/vector4b.hpp>

namespace natus 
{
    namespace collide
    {
        namespace n2d
        {
            template< typename T >
            struct hit_test_aabb_circle
            {
                typedef T type_t ;
                natus_typedefs( aabb< type_t >, aabb ) ;
                natus_typedefs( circle< type_t >, circle ) ;
                natus_typedefs( natus::math::vector2< type_t >, vec2 ) ;
                natus_typedefs( natus::math::vector4< type_t >, vec4 ) ;

                /// @return true, if circle and aabb overlap, otherwise false.
                static bool_t test_overlap( aabb_cref_t bound_aabb, circle_cref_t bound_circle ) noexcept
                {
                    const type_t d = bound_aabb.squared_distance_to( bound_circle.get_center() ) ;
                    const type_t r = bound_circle.get_radius2() ;

                    return d <= r ;
                }

                /// @return true, if circle and aabb overlap, otherwise false.
                /// @return closest point to circle on intersection. Invalid if function returns false.
                static bool_t test_overlap( aabb_cref_t bound_aabb, circle_cref_t bound_circle, vec2_out_t closest_point ) noexcept
                {
                    bound_aabb.closest_point_to( bound_circle.get_center(), closest_point ) ;
                    return (closest_point - bound_circle.get_center()).length2()  <= bound_circle.get_radius2() ;
                }

                /// box against circle
                /// @return 
                /// outside: if box and sphere are completely outside of each other
                /// inside: the box is completely inside of the sphere
                /// intersect: both box' and sphere's surface intersect
                static hit_test_type test_full( aabb_cref_t volume_a, circle_cref_t volume_b ) noexcept
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
            };
        }
    }
}