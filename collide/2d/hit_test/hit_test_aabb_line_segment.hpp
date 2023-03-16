#pragma once

#include "../../hit_test_types.h"
#include "../bounds/aabb.hpp"

#include <natus/math/primitive/2d/line_segment.hpp>

#include <limits>

//
// This file is obsolete
//

namespace natus
{
    namespace collide 
    {
        namespace n2d
        {
            using natus::collide::n2d::aabb ;
            using natus::math::m2d::line_segment ;

            // @obsolete see hit_tes_aabb.hpp

            /// box/ray
            /// algorithm from rtr p. 743
            /// this test doesnt return the distance to the intersection point. there is a similar function though, which does that.
            /// @precondition
            /// the ray and the box need to be in the same space.
            /// the ray need to be normalized in direction.
            /// @return 
            /// ht_outside: the ray does not intersect with the aabb box
            /// ht_intersect: the ray and the box intersect
            template< typename type_t >
            static hit_test_type hit_test_box_line_segment( 
                aabb< type_t > const & box, 
                line_segment<type_t> const & ls )
            {
                typedef natus::math::vector2< type_t > vec2_t ;
                typedef natus::math::vector2b vec2b_t ;

                const vec2_t w = ls.get_distance_half() ;
                const vec2_t v = w.absed() ;

                const vec2_t c = ls.get_center() - box.get_center() ;
                const vec2_t h = vec2_t(box.get_max() - box.get_center()).abs() ;

                if( vec2b_t( c.absed().greater_than(v+h) ).any() ) return natus::collide::hit_test_type::outside ; 
                if( c.det(w) > (h.x()*v.y()+h.y()*v.x()) ) return natus::collide::hit_test_type::outside ; 

                return natus::collide::hit_test_type::intersect ;
            }
        }
    }
    
}