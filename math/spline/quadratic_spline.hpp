#pragma once

#include "../typedefs.h"
#include "continuity.h"
#include "../interpolation/interpolate.hpp"

#include <natus/ntd/vector.hpp>

namespace natus
{
    namespace math
    {
        // qudratic bezier
        template< typename T >
        class quadratic_spline
        {
            natus_this_typedefs( quadratic_spline<T> ) ;
            natus_typedefs( T, type ) ;    
            natus_typedefs( T, value ) ;

            template< typename T2 >
            struct segment
            {
                T2 p0 ;
                T2 p1 ;
                T2 p2 ;
            };
            natus_typedefs( segment< float_t >, segmentf ) ;
            natus_typedefs( segment< value_t >, segmentv ) ;

        private:

            /// control point being evaluated
            natus::ntd::vector< value_t > _cps ;

        public:

            enum class init_type
            {
                // take control points as passed.
                complete,
                // take given control points and construct a c1 continous spline
                construct_c1,

            };

        public:

            quadratic_spline( void_t ) noexcept
            {}

            // init with first segment
            quadratic_spline( value_cref_t p0, value_cref_t p1, value_cref_t p2 ) noexcept
            {
                _cps.emplace_back( p0 ) ;
                _cps.emplace_back( p1 ) ;
                _cps.emplace_back( p2 ) ;
            }

            // pass control points
            // if t == construct_c1 -> first three control points represent the first segment. Every other control point
            // is interpreted as interpolating points
            // @precondition li.size() >= 3
            quadratic_spline( natus::ntd::vector< value_t > const & li, this_t::init_type t = this_t::init_type::complete ) noexcept
            {
                if( li.size() < 3 ) return ;

                if( t == this_t::init_type::complete )
                {
                    for( auto const & i : li )
                    {
                        _cps.emplace_back( i ) ;
                    }
                }
                else if( t == this_t::init_type::construct_c1 )
                {
                    _cps.reserve( li.size() * 2 ) ;
                    
                    _cps.emplace_back( li[0] ) ;
                    _cps.emplace_back( li[1] ) ;
                    _cps.emplace_back( li[2] ) ;


                    for( size_t i=3; i<li.size(); ++i )
                    {
                        size_t const s = this_t::num_segments() - 1 ;

                        size_t const base = s * 2 ;

                        auto const p0 = _cps[ base + 0 ] ;
                        auto const p1 = _cps[ base + 1 ] ;
                        auto const p2 = _cps[ base + 2 ] ;

                        _cps.emplace_back( _cps.back() + natus::math::interpolation<value_t>::quadratic_dt( p0, p1, p2, 1.0f )*0.5f ) ;

                        _cps.emplace_back( li[i] ) ;
                    }
                }
            }

            quadratic_spline( std::initializer_list< value_t > const & li, this_t::init_type t = this_t::init_type::complete ) noexcept :
                quadratic_spline( natus::ntd::vector< value_t >( li), t )
            {
                
            }
            quadratic_spline( this_rref_t rhv ) noexcept
            {
                (*this) = std::move(rhv) ;
            }

            quadratic_spline( this_cref_t rhv ) noexcept
            {
                (*this) = rhv ;
            }

            this_ref_t operator = ( this_rref_t rhv ) noexcept
            {
                _cps = std::move( rhv._cps ) ;
                return ( *this ) ;
            }

            this_ref_t operator = ( this_cref_t rhv ) noexcept
            {
                _cps = rhv._cps ;
                return ( *this ) ;
            }

        public:

            void_t clear( void_t ) noexcept
            {
                _cps.clear() ;
            }

            // Add an interpolating control point.
            // computes the mid control point in c1 continuity.
            // Computes c1 if at least three points are present.
            void append( value_in_t cp ) noexcept
            {
                if( this_t::ncp() < 3 ) 
                {
                    _cps.emplace_back( cp ) ;
                    return ;
                }

                size_t const s = this_t::num_segments() - 1 ;

                size_t const base = s * 2 ;

                auto const p0 = _cps[ base + 0 ] ;
                auto const p1 = _cps[ base + 1 ] ;
                auto const p2 = _cps[ base + 2 ] ;

                _cps.emplace_back( _cps.back() + natus::math::interpolation<value_t>::quadratic_dt( p0, p1, p2, 1.0f )*0.5f ) ;

                _cps.emplace_back( cp ) ;
            }

            /// return the number of segments.
            size_t num_segments( void_t ) const noexcept
            {
                // (ncp - 1) / p
                // where p is the power of the spline. quadratic => 2
                return (_cps.size() - 1) / 2 ; 
            }

            /// return the number of segments.
            size_t get_num_segments( void_t ) const noexcept
            {
                return this_t::num_segments() ;
            }

            size_t ns( void_t ) const noexcept
            {
                return this_t::num_segments() ;
            }

            // quadratic spline has 3 points per segment (pps)
            size_t points_per_segment( void_t ) const noexcept
            {
                return 3 ;
            }

            size_t pps( void_t ) const noexcept
            {
                return this_t::points_per_segment() ;
            }

            // number of control points
            size_t ncp( void_t ) const noexcept
            {
                return _cps.size() ;
            }

            // evaluate at global t.
            // requires 3 control points at min.
            bool_t operator() ( float_t const t_g, value_out_t val_out ) const noexcept
            {
                if( this_t::ncp() < 3 ) return false ;
                auto const t = std::min( 1.0f, std::max( t_g, 0.0f ) ) ;

                float_t const local_t = this_t::global_to_local( t ) ;
                auto const seg = this_t::get_segment( t ) ;

                val_out = natus::math::interpolation<value_t>::quadratic( seg.p0, seg.p1, seg.p2, local_t ) ;

                return true ;
            }

            // evaluates at global t.
            value_t operator() ( float_t const t ) const noexcept
            {
                value_t ret ;
                this_t::operator()( t, ret ) ;
                return ret ;
            }

            // evaluates 1st differential at global t.
            value_t dt( float_t const t ) const noexcept
            {
                if( this_t::ncp() < 3 ) return value_t() ;

                float_t const local_t = this_t::global_to_local( t ) ;
                auto const seg = this_t::get_segment( t ) ;

                return natus::math::interpolation<value_t>::quadratic_dt( seg.p0, seg.p1, seg.p2, local_t ) ;
            }

        private:

            // convert global t to segment index
            // @param tg global t E [0.0,1.0]
            // @return segment index E [0,ns-1]
            size_t segment_index( float_t const t ) const noexcept
            {
                return size_t( std::min( std::floor( t * float_t( this_t::ns() ) ), float_t( this_t::ns() - 1 ) ) ) ;
            }

            // returns the value segment for a global t.
            segmentv_t get_segment( float_t const t_g ) const noexcept
            {
                auto const si = this_t::segment_index( t_g ) ;
                return this_t::get_segment( si ) ;
            }

            segmentv_t get_segment( size_t const si ) const noexcept
            {
                size_t const i = si * (this_t::pps()-1) ;
                return this_t::segmentv_t( { _cps[i+0], _cps[i+1], _cps[i+2]} ) ;
            }

            // returns the segemnt of the t values for the involved cps
            // in the requested segment. This is used to compute the 
            // local t value.
            segmentf_t get_t_segment( size_t const si ) const noexcept
            {
                float_t const b = float_t( si * (this_t::pps() - 1) ) ;
                float_t const r = 1.0f / float_t(this_t::ncp() - 1) ;
                return segmentf_t{ (b + 0.0f)*r, (b + 1.0f)*r, (b + 2.0f)*r };
            }

            float_t global_to_local( float_t const t ) const noexcept
            {
                size_t const si = this_t::segment_index( t ) ;
                auto const st = this_t::get_t_segment( si ) ;
                return this_t::global_to_local( t, st ) ;
            }

            float_t global_to_local( float_t const t, segmentf_cref_t s ) const noexcept
            {
                return (t - s.p0) / (s.p2 - s.p0) ;
            }

            bool_t is_in_range( float_t const t ) const noexcept
            {
                return t >= 0.0f && t <= 1.0f ;
            }

        } ;
        natus_typedefs( quadratic_spline<float_t>, quadratic_splinef ) ;
    }
}