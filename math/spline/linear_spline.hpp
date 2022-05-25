#pragma once

#include "../typedefs.h"

#include "../interpolation/interpolate.hpp"
#include <natus/ntd/vector.hpp>

#include <algorithm>

namespace natus
{
    namespace math
    {
        template< typename T > 
        class linear_spline
        {
            natus_this_typedefs( linear_spline<T> ) ;
            natus_typedefs( T, type ) ;    
            natus_typedefs( type_t, value ) ;

            template< typename T2 >
            struct segment
            {
                T2 p0 ;
                T2 p1 ;
            };
            natus_typedefs( segment< float_t >, segmentf ) ;
            natus_typedefs( segment< value_t >, segmentv ) ;
        
        private:

            /// control point being evaluated
            natus::ntd::vector< value_t > _cps ;

        public:

            linear_spline( void_t ) noexcept
            {}

            linear_spline( value_cref_t p0, value_cref_t p1 ) noexcept
            {
                _cps.emplace_back( p0 ) ;
                _cps.emplace_back( p1 ) ;
            }

            linear_spline( std::initializer_list< value_t > const & il ) noexcept
            {
                for( auto const & i : il )
                {
                    _cps.emplace_back( i ) ;
                }                
            }

            linear_spline( this_rref_t rhv ) noexcept
            {
                (*this) = std::move(rhv) ;
            }

            linear_spline( this_cref_t rhv ) noexcept
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

            /// Adds a control point to the end of the control point array.
            void push_back( value_in_t cp ) noexcept
            {
                _cps.push_back( cp ) ;
            }
        
            /// replaced the control point at index i
            bool_t replace( size_t i, value_in_t new_val, value_out_t old_val ) noexcept
            {
                if( i >= _cps.size() ) return false ;

                old_val = _cps[i] ;
                _cps[i] = new_val ;
            
                return true ;
            }
        
            /// Inserts the passed control point before the 
            /// control point at index i.        
            bool_t insert( size_t const index, value_cref_t cp ) noexcept
            {
                if( index >= _cps.size() ) return false ;

                _cps.insert( _cps.cbegin() + index, cp ) ;

                return true ;
            }

            /// returns the control point at index
            bool_t get_control_point( size_t const index, value_out_t val_out ) const noexcept
            {
                if( index >= _cps.size() ) return false ;

                val_out = _cps[ index ] ;

                return true ;
            }
        
            /// Returns the number of control points.
            size_t num_control_points( void_t ) const noexcept
            {
                return _cps.size() ;
            }

            /// Returns the number of control points.
            size_t get_num_control_points( void_t ) const noexcept
            {
                return _cps.size() ;
            }

            size_t ncp( void_t ) const noexcept
            {
                return _cps.size() ;
            }

            /// return the number of segments.
            size_t ns( void_t ) const noexcept
            {
                // (npc - 1) / p where p:=1 for linear splines
                return this_t::ncp() - 1 ;
            }
        
            /// return the number of segments.
            size_t num_segments( void_t ) const noexcept
            {
                return this_t::ns() ;
            }

            /// return the number of segments.
            size_t get_num_segments( void_t ) const noexcept
            {
                return this_t::ns() ;
            }

            // linear spline has 2 points per segment (pps)
            size_t points_per_segment( void_t ) const noexcept
            {
                return 2 ;
            }

            size_t pps( void_t ) const noexcept
            {
                return this_t::points_per_segment() ;
            }

            /// evaluate at global t.
            /// the linear spline requires at least 2 control points to work.
            bool_t operator() ( float_t const t_g, value_out_t val_out ) const noexcept
            {
                if( this_t::ncp() < 2 ) return false ;
                auto const t = std::min( 1.0f, std::max( t_g, 0.0f ) ) ;

                float_t const local_t = this_t::global_to_local( t ) ;
                auto const seg = this_t::get_segment( t ) ;

                val_out = natus::math::interpolation<value_t>::linear( seg.p0, seg.p1, local_t ) ;

                return true ;
            }

            value_t operator() ( float_t const t ) const noexcept
            {
                value_t ret ;
                this_t::operator()( t, ret ) ;
                return ret ;
            }

            /// 
            bool_t dt( float_t const t, value_out_t val_out ) const noexcept
            {
                if( this_t::ncp() < 2 ) return false ;

                segment_t const seg = this_t::__get_segment( this_t::segment_index( t ) ) ;

                val_out = seg.second - seg.first ;

                return true ;
            }

        private:

            /// convert global t to segment index
            /// @param tg global t E [0.0,1.0]
            /// @return segment index E [0,ns-1]
            size_t segment_index( float_t const t_g ) const noexcept
            {
                return size_t( std::min( std::floor(t_g*this_t::ns()), float_t(this_t::ns()-1) ) ) ;
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
                return this_t::segmentv_t( { _cps[i+0], _cps[i+1] } ) ;
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

            bool_t is_in_range( float_t const t, value_out_t vout ) const noexcept
            {
                if( t <= 0.0f )
                {
                    vout = _cps[ 0 ] ;
                    return false ;
                }
                else if( t >= 1.0f )
                {
                    vout = _cps[ this_t::get_num_control_points() - 1 ] ;
                    return false ;
                }

                return true ;
            }
        } ;
    }
}  // namespace 




