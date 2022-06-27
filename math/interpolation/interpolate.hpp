#pragma once

#include "../typedefs.h"

namespace natus
{
    namespace math
    {
        template< typename T >
        struct interpolation
        {
            natus_typedefs( T, value ) ;
            typedef float_t real_t ;

            static value_t linear( value_cref_t p1, value_cref_t p2, real_t const& t )
            {
                return value_t( p1 * ( real_t( 1 ) - t ) + p2 * t );
            }

            static value_t quadratic( value_cref_t p1, value_cref_t p2,
                value_cref_t p3, real_t const& t )
            {
                // one minus t
                real_t const omt = real_t( 1 ) - t ;
                real_t const omt2 = omt * omt ;

                auto const term1 = p2 ;
                auto const term2 = ( p1 - p2 ) * omt2 ;
                auto const term3 = ( p3 - p2 ) * t * t ;

                return term1 + term2 + term3 ;
            }

            static value_t quadratic_dt( value_cref_t p1, value_cref_t p2,
                value_cref_t p3, real_t const& t )
            {
                real_t const omt = real_t(1) - t;

                auto const term1 = (p2 - p1) * omt * real_t(2) ;
                auto const term2 = (p3 - p2) * t * real_t(2) ;

                return term1 + term2 ;
            }

            static value_t cubic( value_cref_t p1, value_cref_t p2,
                value_cref_t p3, value_cref_t p4, real_t const& t )
            {
                real_t const one_minus_t = ( real_t( 1 ) - t ) ;
                real_t const one_minus_t2 = one_minus_t * one_minus_t ;
                real_t const one_minus_t3 = one_minus_t2 * one_minus_t ;
                real_t const t2 = t * t ;
                real_t const t3 = t2 * t;

                // b(t) = (1-t)^3*p1 + 3*(1-t)^2*t*p2 + 3*(1-t)*t^2*p3 + t^3*p4
                auto const term1 = p1 * one_minus_t3 ;
                auto const term2 = p2 * one_minus_t2 * t * real_t(3) ;
                auto const term3 = p3 * one_minus_t * t2 * real_t(3) ;
                auto const term4 = p4 * t3 ;

                return term1 + term2 + term3 + term4 ;
            }

            static value_t cubic_dt( value_cref_t p1, value_cref_t p2,
                value_cref_t p3, value_cref_t p4, real_t const& t )
            {
                real_t const t2 = t * t ;
                real_t const one_minus_t = ( real_t( 1 ) - t ) ;
                real_t const one_minus_t2 = one_minus_t * one_minus_t ;

                auto const s1 = p2 - p1 ;
                auto const s2 = p3 - p2 ;
                auto const s3 = p4 - p3 ;

                auto const term1 = s1 * one_minus_t2 * real_t(3) ;
                auto const term2 = s2 * one_minus_t * t * real_t(6) ;
                auto const term3 = s3 * t2 * real_t( 3 ) ;

                return term1 + term2 + term3 ;
            }

            static value_t cubic_dt2( value_cref_t p1, value_cref_t p2,
                value_cref_t p3, value_cref_t p4, real_t const& t )
            { 
                real_t const t2 = t * t ;
                real_t const one_minus_t = real_t( 1 ) - t ;
                real_t const one_minus_t2 = one_minus_t * one_minus_t ;

                auto const s1 = p3 - p2 * real_t(2) + p1 ;
                auto const s2 = p4 - p3 * real_t(2) + p2 ;

                auto const term1 = s1 * one_minus_t * real_t(6) ;
                auto const term2 = s2 * t * real_t(6) ;

                return term1 + term2 ;
            }

            static value_t catrom_by_slope( value_cref_t p1, value_cref_t m1,
                value_cref_t p2, value_cref_t m2, real_t const& t )
            {
                real_t t2 = t * t ;
                real_t t3 = t2 * t ;

                real_t s3_t2 = real_t( 3 ) * t2 ; // 3*t^2
                real_t s2_t3 = real_t( 2 ) * t3 ; // 2*t^3

                // p(t) = (2t^3-3t^2+1) * p1 + (t^3-2*t^2+t) * m1 + (-2t^3+3t^2) * p2 + (t^3-t^2) * m2 
                return
                    p1 * ( s2_t3 - s3_t2 + real_t( 1 ) ) + m1 * ( t3 - real_t( 2 ) * t2 + t ) +
                    p2 * ( -s2_t3 + s3_t2 ) + m2 * ( t3 - t2 ) ;
            }

            static value_t catrom_by_point( value_cref_t p1, value_cref_t p2,
                value_cref_t p3, value_cref_t p4, real_t const& t )
            {
                value_t const m2 = p3 - p1 ; // slope at p2
                value_t const m3 = p4 - p2 ; // slope at p3

                return catrom_interp_slope( p2, m2, p3, m3 ) ;
            }
        };
    }
}