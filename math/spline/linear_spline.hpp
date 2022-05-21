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

            typedef std::pair< value_t, value_t > __segment_t ;
            natus_typedefs( __segment_t, segment ) ;

        
        private:

            /// control point being evaluated
            natus::ntd::vector< value_t > _cps ;

            /// number of segments.
            size_t _ns = 0 ;

            /// reciprocal of number of segments.
            float_t _rns = 0.0f ;

        public:

            linear_spline( void_t ) noexcept
            {}

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
                _ns = rhv._ns ;
                _rns = rhv._rns ;
                return ( *this ) ;
            }

            this_ref_t operator = ( this_cref_t rhv ) noexcept
            {
                _cps = rhv._cps ;
                _ns = rhv._ns ;
                _rns = rhv._rns ;
                return ( *this ) ;
            }

        public:
        
            void_t clear( void_t ) noexcept
            {
                _cps.clear() ;
                _ns = 0 ;
                _rns = 0.0f ;
            }

            /// Adds a control point to the end of the control point array.
            void push_back( value_in_t cp ) noexcept
            {
                _cps.push_back( cp ) ;
                _ns =  _cps.size() - 1 ;
                _rns = float_t(1)/float_t(_ns) ;
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

                // recalculate internal variables.
                _ns =  _cps.size() - 1 ;
                _rns = float_t(1)/float_t(_ns) ;

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

        
            /// return the number of segments.
            size_t num_segments( void_t ) const noexcept
            {
                return _ns ;
            }

            /// return the number of segments.
            size_t get_num_segments( void_t ) const noexcept
            {
                return _ns ;
            }

            /// evaluate at global t.
            /// the linear spline requires at least 2 control points to work.
            bool_t operator() ( float_t const t, value_out_t val_out ) const noexcept
            {
                if( _ns < 1 ) return false ;
            
                if( natus::core::is_not( this_t::is_in_range( t ) ) ){
                    return false ;
                }

                // get the segment of the given global t.
                size_t const si = this_t::segment_index( t ) ;

                // get the control point for the corresponding 
                // segment.
                segment_t const seg = this_t::__get_segment( si ) ;

                // convert the global t into the local t in the segment space.
                float_t const local_t = this_t::global_to_local( t, si ) ;

                // now, lerp across the points.
                //return cps.first * (float_t(1)-local_t) + cps.second * local_t ;
                val_out = natus::math::interpolation<value_t>::linear( seg.first, seg.second, local_t ) ;

                return true ;
            }

            /// 
            bool_t dt( float_t const t, value_out_t val_out ) const noexcept
            {
                if( _ns < 1 ) return false ;

                segment_t const seg = this_t::__get_segment( this_t::segment_index( t ) ) ;

                val_out = seg.second - seg.first ;

                return true ;
            }

            /// convert global t to segment index
            /// @param tg global t E [0.0,1.0]
            /// @return segment index E [0,ns-1]
            size_t segment_index( float_t const t_g ) const noexcept
            {
                return size_t( std::min( std::floor(t_g*_ns), float_t(_ns-1) ) ) ;
            }

            /// Converts the global t into a t that is local to the current 
            /// segment. For example, if the spline has 5 control point where the
            /// first point start at t=0.0 and the last ends with t=1.0, that t must
            /// be recalculated so that it fits into the current segment where the 
            /// t is right now.       
            float_t global_to_local( float_t const t ) const noexcept
            {
                return this_t::global_to_local( t, this_t::segment_index( t ) ) ;
            }


            /// Optimized version. Here the segment index can be passed
            /// as a parameter, so it must not be recalculated. This is 
            /// of use in a function where the segment index is still calculated.
            float_t global_to_local( float_t const t, size_t const si ) const noexcept
            {
                // could be written as: t*ns-si
                // could be written as: t*ns-floor(t*ns)
                return (t - (si*_rns))*_ns ;
            }

        private:

            /// private version without check
            segment_t __get_segment( size_t const s ) const noexcept
            {
                return segment_t( _cps[s], _cps[s+1] ) ;
            }

        
            /// Returns the segments control points. The segment index is in [0,s] 
            /// where s is the number of segments-1.
            bool_t get_segment( size_t const seg, segment_out_t seg_out ) const noexcept
            {
                if( (seg+1) >= _cps.size() ) return false ;
            
                seg_out = this_t::__get_segment( seg ) ;

                return true ;
            }

            float_t t_to_valid_range( float_t const t ) const noexcept
            {
                return std::min( std::max(0.0f,t), 1.0f ) ;
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




