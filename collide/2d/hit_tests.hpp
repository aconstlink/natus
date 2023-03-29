#pragma once

#include "../typedefs.h"
#include "../hit_test_types.h"

#include "bounds/aabb.hpp"
#include "bounds/obb.hpp"
#include "bounds/circle.hpp"

#include <natus/math/primitive/2d/ray.hpp>
#include <natus/math/vector/vector4.hpp>
#include <natus/math/vector/vector4b.hpp>

#include <limits>

namespace natus 
{
    namespace collide
    {
        namespace n2d
        {
            template< typename T >
            struct hit_tests
            {
                natus_this_typedefs( hit_tests< T > ) ;

                natus_typedefs( T, type ) ;
                natus_typedefs( natus::math::m2d::ray< T >, ray ) ;
                natus_typedefs( aabb< type_t >, aabb ) ;
                natus_typedefs( obb< type_t >, obb ) ;
                natus_typedefs( circle< type_t >, circle ) ;
                natus_typedefs( natus::math::vector2< type_t >, vec2 ) ;
                natus_typedefs( natus::math::vector3< type_t >, vec3 ) ;
                natus_typedefs( natus::math::vector4< type_t >, vec4 ) ;

                static hit_test_type aabb_aabb_overlap( aabb_cref_t b0, aabb_cref_t b1 ) noexcept
                {
                    {
                        auto const m0 = b0.get_min() ;
                        auto const m1 = b1.get_max() ;
                        if( m1.less_than( m0 ).any() ) return hit_test_type::disjoint ;
                    }

                    {
                        auto const m0 = b1.get_min() ;
                        auto const m1 = b0.get_max() ;
                        if( m1.less_than( m0 ).any() ) return hit_test_type::disjoint ;
                    }

                    return hit_test_type::overlap ;
                }

                static hit_test_type aabb_aabb( aabb_cref_t b0, aabb_cref_t b1, 
                    vec2_out_t cp, vec3_out_t nrm, size_t & np ) noexcept
                {
                    return hit_test_type::disjoint ;
                }

                // @return true, if circle and aabb overlap, otherwise false.
                // used optimized technique from aabb_circle function below.
                static hit_test_type aabb_circle_overlap( aabb_cref_t box, circle_cref_t circ ) noexcept
                {
                    vec2_t const e = box.get_extend() ;
                    vec2_t const c = (circ.get_center() - box.get_center()).abs() ;
                    vec2_t const d = c - e ;
                    type_t const r = circ.get_radius() ;

                    return d.greater_than( vec2_t( r ) ).any() ? hit_test_type::disjoint : hit_test_type::overlap ;
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
                static hit_test_type aabb_circle( aabb_cref_t box, circle_cref_t circ ) noexcept
                {
                    #if 0 // nice one
                    const vec2_t max = circ.get_center() - box.get_max() ;
                    const vec2_t min = circ.get_center() - box.get_min() ;
                    const type_t radius = circ.get_radius() ;

                    // test full outside
                    {
                        auto const minb = min.less_than( vec2_t( -radius ) ) ;
                        auto const maxb = max.greater_than( vec2_t( radius ) ) ;

                        if( minb.any() || maxb.any() ) return hit_test_type::outside ;
                    }

                    // test intersection
                    // note that due to the first test,
                    // that min must be greater than -radius and
                    // that max must be smaller than radius
                    {
                        auto const minb = min.less_than( vec2_t( radius ) ) ;
                        auto const maxb = max.greater_than( vec2_t( -radius ) ) ;

                        if( minb.any() || maxb.any() ) return hit_test_type::intersect ;
                    }
                    
                    #else // optimized one
                    // d is the dot product of the two normals 
                    // facing the circle in one shot.

                    vec2_t const e = box.get_extend() ;
                    vec2_t const c = (circ.get_center() - box.get_center()).abs() ;
                    vec2_t const d = c - e ;
                    type_t const r = circ.get_radius() ;

                    auto const b = vec4_t( d, d ).greater_than( vec4_t( vec2_t( +r ), vec2_t( -r ) ) ) ;

                    if( b.xy().any() ) return hit_test_type::outside ;
                    if( b.zw().any() ) return hit_test_type::intersect ;
                    #endif

                    return hit_test_type::inside ;
                }

                // hit tests aabb vs circle and if penetration happend, 
                // the contact point and normal is returned.
                static hit_test_type aabb_circle( aabb_cref_t box, circle_cref_t circle, 
                    vec2_out_t cp, vec3_out_t nrm, size_t & np ) noexcept
                {
                    np = 0 ;

                    #if 0 
                    auto const res = this_t::aabb_circle( box, circle ) ;
                    if( res == hit_test_type::outside ) return res ;                    
                    #else // this test takes even less condition tests
                    auto const res = this_t::aabb_circle_overlap( box, circle ) ;
                    if( res == hit_test_type::disjoint) return res ;
                    #endif

                    nrm = box.calculate_normal_for( circle.get_center() ) ;
                    cp = circle.get_center() - nrm.xy() * 
                        nrm.dot( natus::math::vec3f_t( circle.get_center() - box.get_center(), 1.0f ) ) ;
                    np = 1 ;

                    return res ;
                }

                // obb vs circle. 
                // transforms circle into obb space and solves aabb vs circle.
                static hit_test_type obb_circle( obb_cref_t box, circle_cref_t circ ) noexcept
                {
                    auto const cc = box.ref_to_local() * vec3_t( circ.get_center(), 1.0 ) ;

                    auto const ext = box.get_extend() ;
                    auto const min = -ext ;
                    auto const max = +ext ;

                    return this_t::aabb_circle( aabb_t( min, max ), circle_t( cc, circ.get_radius() ) ) ;
                }

                static hit_test_type obb_circle( obb_cref_t box, circle_cref_t circ, 
                    vec2_out_t cp, vec3_out_t nrm, size_t & np ) noexcept
                {
                    auto const cc = box.ref_to_local() * vec3_t( circ.get_center(), 1.0 ) ;

                    auto const res = this_t::aabb_circle( aabb_t( box.get_minmax() ), circle_t( cc, circ.get_radius() ),
                        cp, nrm, np ) ;

                    if( res == hit_test_type::outside ) return res ;

                    cp = box.local_to_ref_point( cp ) ; 
                    nrm = vec3_t( box.local_to_ref_direction( nrm.xy() ), nrm.z() ) ;

                    return res ;
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
                    type_t const b = m.dot( ray.get_direction() ) ;
                    type_t const c = m.dot( m ) - circ.get_radius2() ;

                    // ray and circle disjoint(c >0 ) and ray pointing away (b>0)
                    if( c > 0.0f && b > 0.0f ) return hit_test_type::outside ;

                    type_t const discr = b*b - c ;

                    // no real solution, no intersection
                    if( discr < 0.0f ) return hit_test_type::outside ;

                    type_t s = std::sqrt( discr ) ;

                    type_t const x = -b - s ;
                    type_t const y = -b + s ;
                    
                    ds = x > 0.0f ? vec2_t( x, y ) : vec2_t( y, x ) ;

                    return ds.less_than( 0.0f ).any() ? hit_test_type::inside : hit_test_type::intersect ;
                }

                static hit_test_type ray_aabb( ray_cref_t ray, aabb_cref_t box, type_out_t l, vec3_out_t nrm_out ) noexcept
                {
                    auto const nrms = box.calculate_normals_for_direction( ray.get_direction() ) ;

                    {
                        auto const n = nrms[0] ;
                        auto const a = n.dot( vec3_t( ray.get_origin() - box.get_center(), type_t(1) ) ) ;
                        auto const b = n.xy().dot( ray.get_direction() ) ;

                        l = -a / b ;
                        nrm_out = n ;

                        auto const p = ray.point_at( l ) ;
                        
                        // the "other" normals are othrogonal to the 
                        // direct hit normal and that one other normal 
                        // represents the halfspace of the box, so both
                        // normals are in the same distance to the origin 
                        // of the box but with opposing normals.
                        std::array< vec3_t, 2 > const onrms = 
                        {
                            vec3_t( nrms[1].xy(), nrms[1].z() ),
                            vec3_t( nrms[1].xy().negated(), nrms[1].z() )
                        } ;

                        vec2_t const ds(
                            onrms[0].dot( vec3_t( p - box.get_center(), type_t(1) ) ),
                            onrms[1].dot( vec3_t( p - box.get_center(), type_t(1) ) ) ) ;

                        if( ds.less_than( vec2_t() ).all() ) return hit_test_type::intersect ;
                    }

                    {
                        auto const n = nrms[1] ;
                        auto const a = n.dot( vec3_t( ray.get_origin() - box.get_center(), type_t(1) ) ) ;
                        auto const b = n.xy().dot( ray.get_direction() ) ;

                        l = -a / b ;
                        nrm_out = n ;

                        auto const p = ray.point_at( l ) ;

                        std::array< vec3_t, 2 > const onrms = 
                        {
                            vec3_t( nrms[0].xy(), nrms[0].z() ),
                            vec3_t( nrms[0].xy().negated(), nrms[0].z() )
                        } ;

                        vec2_t const ds(
                            onrms[0].dot( vec3_t( p - box.get_center(), type_t(1) ) ),
                            onrms[1].dot( vec3_t( p - box.get_center(), type_t(1) ) ) ) ;

                        if( ds.less_than( vec2_t() ).all() ) return hit_test_type::intersect ;
                    }

                    return hit_test_type::disjoint ;
                }

                // transforms the ray into obb space, solves for the contact 
                // points and transforms back into reference space.
                static hit_test_type ray_obb( ray_cref_t ray, obb_cref_t box, type_t & l, vec3_out_t nrm_out ) noexcept
                {
                    auto const res = this_t::ray_aabb( ray_t( box.ref_to_local( ray ) ), aabb_t( box.get_minmax() ),
                        l, nrm_out ) ;

                    nrm_out = vec3_t( box.local_to_ref_direction( nrm_out.xy() ), nrm_out.z() ) ;

                    return res ;
                }

                static hit_test_type ray_ray( ray_cref_t r0, ray_cref_t r1, vec2_out_t cp ) noexcept
                {
                    if( std::abs( type_t(1) - r0.dot( r1 ) ) < std::numeric_limits<type_t>::epsilon() ) return hit_test_type::disjoint ;

                    auto const l = r0.lambda_to( r1 ) ;
                    if( l < type_t(0) ) return hit_test_type::disjoint ;

                    cp = r0.point_at( l ) ;

                    return hit_test_type::intersect ;
                }


                #if 0 // old one
                //
                static hit_test_type ray_aabb( ray_cref_t ray, aabb_cref_t box ) noexcept
                {
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
                #endif
            };
        }
    }
}