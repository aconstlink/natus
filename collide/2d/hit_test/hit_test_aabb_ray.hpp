
#pragma once

#include "../../hit_test_types.h"
#include "../bounds/aabb.hpp"

#include <natus/math/vector/vector2.hpp>
#include <natus/math/primitive/2d/ray.hpp>
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

            /// @returns
            /// ht_disjoint
            /// ht_overlap
            /*template< typename type_t >
            static hit_test_type hit_test_overlap( aabb< natus::math::vector3< type_t > > const & box, ray<natus::math::vector3< type_t > > const & ray )
            {

            }*/

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
            static hit_test_type hit_test_box_ray( aabb< type_t > const & box, 
                natus::math::m2d::ray< type_t > const & ray )
            {
                typedef natus::math::vector2< type_t > vec2_t ;

                type_t t_min = std::numeric_limits<type_t>::min() ;
                type_t t_max = std::numeric_limits<type_t>::max() ;

                vec2_t p = box.get_center() - ray.get_origin() ;
                vec2_t h = box.get_max() - box.get_center() ;

                // for each axis
                for( size_t i=0; i<2; ++i )
                {
                    type_t e = p[i] ;
                    type_t f = ray.get_direction()[i] ;
                    if( std::abs(f) > std::numeric_limits<type_t>::epsilon() )
                    {
                        type_t t1 = (e+h[i])/f ;
                        type_t t2 = (e-h[i])/f ;
                        if( t1 > t2 ) 
                        {
                            type_t tmp = t1 ;
                            t1 = t2 ;
                            t2 = tmp ;
                        }
                        if( t1 > t_min ) t_min = t1 ;
                        if( t2 < t_max ) t_max = t2 ;
                        if( t_min > t_max ) return natus::collide::hit_test_type::outside ;
                        if( t_max < 0 ) return natus::collide::hit_test_type::outside ;
                    }
                    else if( -e-h[i] > type_t(0) || -e + h[i] < type_t(0) ) return natus::collide::hit_test_type::outside ;
                }

                return natus::collide::hit_test_type::intersect ;
            }

            // @obsolete see hit_tes_aabb.hpp
            /// box/ray
            /// algorithm from rtr p. 743
            /// this test intersects a box with a ray and returns the distance to the intersection point on the ray.
            /// @precondition
            /// the ray and the box need to be in the same space.
            /// the ray need to be normalized in direction.
            /// @return 
            /// ht_outside: the ray does not intersect with the aabb box
            /// ht_intersect: the ray and the box intersect
            /// param t_out the distance to the intersection point on the ray if box and ray intersects.
            template< typename type_t >
            static hit_test_type hit_test_box_ray( aabb< type_t > const & box, 
                natus::math::m2d::ray< type_t > const & ray, type_t & t_out )
            {
                typedef natus::math::vector2< type_t > vec2_t ;

                t_out = type_t(0) ;

                type_t t_min = std::numeric_limits<type_t>::min() ;
                type_t t_max = std::numeric_limits<type_t>::max() ;

                vec2_t p = box.get_center() - ray.get_origin() ;
                vec2_t h = box.get_max() - box.get_center() ;

                // for each axis
                for( size_t i=0; i<3; ++i )
                {
                    type_t e = p[i] ;
                    type_t f = ray.get_direction()[i] ;
                    if( std::abs(f) > std::numeric_limits<type_t>::epsilon() )
                    {
                        type_t t1 = (e+h[i])/f ;
                        type_t t2 = (e-h[i])/f ;
                        if( t1 > t2 ) 
                        {
                            type_t tmp = t1 ;
                            t1 = t2 ;
                            t2 = tmp ;
                        }
                        if( t1 > t_min ) t_min = t1 ;
                        if( t2 < t_max ) t_max = t2 ;
                        if( t_min > t_max ) return natus::collide::hit_test_type::outside ;
                        if( t_max < 0 ) return natus::collide::hit_test_type::outside ;
                    }
                    else if( -e-h[i] > type_t(0) || -e + h[i] < type_t(0) ) return natus::collide::hit_test_type::outside ;
                }

                t_out = t_min > type_t(0) ? type_t(t_min) : type_t(t_max) ;
                return natus::collide::hit_test_type::intersect ;
            }

            /*template< typename type_t >
            static bool_t intersect( aabb< natus::math::vector3< type_t > > const & box, natus::math::natus_primitive::ray<natus::math::vector3< type_t > > const & ray, type_t & t_out )
            {
                return hit_test_box_ray(box,ray) == ht_intersect ;
            }    */   
        }
    }
}
