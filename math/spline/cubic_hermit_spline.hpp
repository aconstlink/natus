#pragma once

#include "../typedefs.h"
#include "continuity.h"
#include "../interpolation/interpolate.hpp"

#include <natus/ntd/vector.hpp>

namespace natus
{
    namespace math
    {
        template< typename T >
        class cubic_hermit_spline
        {
            natus_this_typedefs( cubic_hermit_spline<T> ) ;
            natus_typedefs( T, type ) ;    
            natus_typedefs( T, value ) ;

        public:
            
            natus_typedefs( natus::ntd::vector< value_t >, points ) ;
            natus_typedefs( natus::ntd::vector< value_t >, tangents ) ;

            struct control_point
            {
                value_t p ;
                value_t lt ;
                value_t rt ;
            } ;
            natus_typedef( control_point ) ;

            template< typename T2 >
            struct segment
            {
                T2 p0 ;
                T2 p1 ;
            };
            natus_typedefs( segment< float_t >, segmentf ) ;
            natus_typedefs( segment< control_point_t >, segmentv ) ;

        private:

            // control point being evaluated
            points_t _cps ;
            tangents_t _lts ;
            tangents_t _rts ;

        public:

            // the first and the last point is used to compute the
            // right and the left tangents of the beginning and the end
            // of the spline control points.
            // the first and the last point is not interpolated and will be
            // dropped when the tangents are computed.
            // all other tangents are computed fromt he convex hull.
            struct init_with_additional_points
            {
            };

            // init the hermit spline with complete data.
            struct init_with_tangents
            {
                // each interpolated point
                points_t points ;
                
                // the left and the right tangents
                // size needs to be points.size() * 2
                tangents_t tangs ;
            };

            // compute tangents catrom-style
            struct init_by_catmull_rom
            {
                // each interpolated point
                points_t points ;
            } ;
            natus_typedef( init_by_catmull_rom ) ;

        public:

            static bool_t construct_spline_data( init_by_catmull_rom_cref_t data, 
                points_out_t points, tangents_out_t lefts, tangents_out_t rights ) noexcept
            {
                if( data.points.size() < 3 ) return false ;

                points.resize( data.points.size() ) ;
                lefts.resize( data.points.size() ) ;
                rights.resize( data.points.size() ) ;

                // frist point
                {
                    auto const m = (data.points[2] - data.points[0]) * 0.5f ;
                    points[0] = data.points[0] ;
                    lefts[0] = m ;
                    rights[0] = m ;
                }

                for( size_t i=1; i<data.points.size()-1; ++i )
                {
                    auto const m = (data.points[i+1] - data.points[i-1]) * 0.5f ; 
                    points[i] = data.points[i] ;
                    lefts[i] = m ;
                    rights[i] = m ;
                }

                // last point
                {
                    size_t const lp = data.points.size() - 1 ;
                    auto const m = (data.points[lp-2] - data.points[lp]) * 0.5f ;
                    points[lp] = data.points[lp] ;
                    lefts[lp] = m ;
                    rights[lp] = m ;
                }

                return true ;
            }

        public:

            cubic_hermit_spline( void_t ) noexcept
            {}

            cubic_hermit_spline( init_by_catmull_rom_cref_t data ) noexcept 
            {
                this_t::construct_spline_data( data, _cps, _lts, _rts ) ;
            }

            // init with first segment
            // for simplicity, the tangents are used for left and right
            cubic_hermit_spline( value_cref_t p0, value_cref_t t0, value_cref_t p1, value_cref_t t1 ) noexcept
            {
            }

            cubic_hermit_spline( this_rref_t rhv ) noexcept
            {
                (*this) = std::move(rhv) ;
            }

            cubic_hermit_spline( this_cref_t rhv ) noexcept
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
                
            }

            void_t change_point( size_t const i, value_cref_t cp ) noexcept
            {
                if( i >= _cps.size() ) return ;

                _cps[i] = cp ;
            }

            /// return the number of segments.
            size_t num_segments( void_t ) const noexcept
            {
                return _cps.size() - 1 ; 
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
                return 2 ;
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
                if( this_t::ncp() < 2 ) return false ;
                auto const t = std::min( 1.0f, std::max( t_g, 0.0f ) ) ;

                float_t const local_t = this_t::global_to_local( t ) ;
                auto const seg = this_t::get_segment( t ) ;

                val_out = natus::math::interpolation<value_t>::cubic_hermit( seg.p0.p, seg.p0.rt, seg.p1.p, seg.p1.lt, local_t ) ;

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
                
            }

        public:

            typedef std::function< void_t ( size_t const, control_point_cref_t ) > for_each_cp_funk_t ;
            void_t for_each_control_point( for_each_cp_funk_t funk ) const noexcept 
            {
                for( size_t i=0; i<_cps.size(); ++i ) funk( i, { _cps[i], _lts[i], _rts[i] } ) ;
            }

            points_cref_t control_points( void_t ) const noexcept
            {
                return _cps ;
            }

            control_point_t get_control_point( size_t const i ) const noexcept 
            {
                return i >= _cps.size() ? control_point_t() : control_point_t { _cps[i], _lts[i], _rts[i] } ;
            }

            // just the points of the control points
            points_cref_t points( void_t ) const noexcept
            {
                return _cps ;
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

                return this_t::segmentv_t {
                    this_t::control_point_t{ _cps[i+0], _lts[i+0], _rts[i+0] } ,
                    this_t::control_point_t{ _cps[i+1], _lts[i+1], _rts[i+1] } } ;
            }

            // returns the segemnt of the t values for the involved cps
            // in the requested segment. This is used to compute the 
            // local t value.
            segmentf_t get_t_segment( size_t const si ) const noexcept
            {
                float_t const b = float_t( si * (this_t::pps() - 1) ) ;
                float_t const r = 1.0f / float_t(this_t::ncp() - 1) ;
                return segmentf_t{ (b + 0.0f)*r, (b + 1.0f)*r };
            }

            float_t global_to_local( float_t const t ) const noexcept
            {
                size_t const si = this_t::segment_index( t ) ;
                auto const st = this_t::get_t_segment( si ) ;
                return this_t::global_to_local( t, st ) ;
            }

            float_t global_to_local( float_t const t, segmentf_cref_t s ) const noexcept
            {
                return (t - s.p0) / (s.p1 - s.p0) ;
            }

            bool_t is_in_range( float_t const t ) const noexcept
            {
                return t >= 0.0f && t <= 1.0f ;
            }

        } ;
        natus_typedefs( cubic_hermit_spline<float_t>, cubic_hermit_splinef ) ;
    }
}