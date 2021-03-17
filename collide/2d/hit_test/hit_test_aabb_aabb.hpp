#pragma once

#include "../../hit_test_types.h"
#include "../bounds/aabb.hpp"

namespace natus
{
    namespace collide
    {
        namespace n2d
        {
            template< typename T >
            struct aabb_aabb_hit_test
            {
                natus_typedefs( natus::collide::n2d::aabb<T>, aabb ) ;

                /// @returns
                /// ht_outside
                /// ht_intersect
                static hit_test_type hit_test_box_box( aabb_cref_t a, aabb_cref_t b ) noexcept
                {
                    if( ( a.get_min().greater_than( b.get_max() ) ).any() ) 
                        return natus::collide::hit_test_type::outside ;

                    if( ( b.get_min().greater_than( a.get_max() ) ).any() ) 
                        return natus::collide::hit_test_type::outside ;

                    return natus::collide::hit_test_type::intersect ;
                }

                static bool_t intersect( aabb_cref_t a, aabb_cref_t b ) noexcept
                {
                    return hit_test_box_box( a, b ) == natus::collide::hit_test_type::intersect ;
                }
            };
        }
    }
}