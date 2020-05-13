#pragma once

#include "protos.hpp"
#include "switch.h"

#include "../typedefs.h"
#include "../utility/fn.hpp"

#include "vector2b.hpp"

#include <algorithm>

namespace natus 
{
    namespace math
    {
        template< typename type >
        class vector2
        {
            natus_this_typedefs( vector2< type > ) ;

        public:

            natus_typedef( type ) ;

            natus_typedefs( vector2<type>, vec2 ) ;
            natus_typedefs( vector3<type>, vec3 ) ;
            natus_typedefs( vector4<type>, vec4 ) ;

            natus_typedefs( vector2b, vecb ) ;

        public:

            //***************************************************
            vector2( void )
            {
                _elem[0] = type_t(0) ;
                _elem[1] = type_t(0) ;
            }

            //***************************************************
            vector2( type_t s )
            {
                _elem[0] = s ;
                _elem[1] = s ;
            }

            //***************************************************
            vector2( type_t x, type_t y )
            {
                (*this)(x,y) ;
            }

            //***************************************************
            vector2( this_cref_t rhv ) 
            {
                (*this)(rhv.x(),rhv.y()) ;
            }

            //***************************************************
            template< typename other_t >
            vector2( vector2<other_t> const & rhv ) 
            {
                (*this)(type_t(rhv.x()),type_t(rhv.y())) ;
            }

            //***************************************************
            vector2( vec3_t const & rhv ) 
            {
                (*this)( rhv.x(), rhv.y() ) ;
            }

            //***************************************************
            vector2( vec4_t const & rhv ) {
                (*this)(rhv.x(),rhv.y()) ;
            }

            vector2( type_t angle, natus::math::cos_sin )
            {
                _elem[0] = std::cos( angle ) ;
                _elem[1] = std::sin( angle ) ;
            }

            vector2( type_t angle, natus::math::sin_cos )
            {
                _elem[0] = std::sin( angle ) ;
                _elem[1] = std::cos( angle ) ;
            }

        public: // x,y,z accessor

            //***************************************************
            type_cref_t x( void ) const {
                return _elem[0] ;
            }

            //***************************************************
            type_cref_t y( void ) const {
                return _elem[1] ;
            }

            //***************************************************
            type_ref_t x( void ) {
                return _elem[0] ;
            }

            //***************************************************
            type_ref_t y( void ) {
                return _elem[1] ;
            }

            //***************************************************
            this_ref_t x( type_cref_t val ){
                _elem[0] = val ;
                return *this ;
            }

            //***************************************************
            this_ref_t y( type_cref_t val ){
                _elem[1] = val ;
                return *this ;
            }

            //***************************************************
            this_t xx( void_t ) const
            {
                return this_t( _elem[ 0 ], _elem[ 0 ] ) ;
            }

            //***************************************************
            this_t yy( void_t ) const 
            {
                return this_t( _elem[ 1 ], _elem[ 1 ] ) ;
            }

        public: // operator ()

            //***************************************************
            this_cref_t operator()( void ){
                _elem[0] = type_t(0);_elem[1] = type_t(0);
                return (*this) ;
            }

            //***************************************************
            this_cref_t operator()( type_t x, type_t y ){
                _elem[0]=x; _elem[1]=y;
                return *this ;
            }

        public: // operator [] // optimized

            //***************************************************
            type_cref_t operator[]( size_t index ) const{
                return _elem[index] ;
            }

            //***************************************************
            type_ref_t operator[]( size_t index ) {
                return _elem[index] ;
            }

        public: // operator +

            //***************************************************
            this_cref_t operator += ( this_cref_t rhv ){
                _elem[0]+=rhv.x(); _elem[1]+=rhv.y(); 
                return (*this) ;
            }

            //***************************************************
            this_t operator + ( this_cref_t rhv ) const{
                return this_t(_elem[0]+rhv.x(), _elem[1]+rhv.y()) ;
            }

            //***************************************************
            this_cref_t operator += ( type_cref_t rhv ){
                _elem[0]+=rhv; _elem[1]+=rhv;
                return (*this) ;
            }

            //***************************************************
            this_t operator + ( type_cref_t rhv ) const{
                return this_t(_elem[0]+rhv, _elem[1]+rhv) ;
            }

        public: // operator -

            //***************************************************
            this_cref_t operator -= ( this_cref_t rhv ){
                _elem[0]-=rhv.x(); _elem[1]-=rhv.y(); 
                return (*this) ;
            }

            //***************************************************
            this_t operator - ( this_cref_t rhv ) const{
                return this_t(_elem[0]-rhv.x(), _elem[1]-rhv.y()) ;
            }

            //***************************************************
            this_cref_t operator -= ( type_cref_t rhv ){
                _elem[0]-=rhv; _elem[1]-=rhv; 
                return (*this) ;
            }

            //***************************************************
            this_t operator - ( type_cref_t rhv ) const{
                return this_t(_elem[0]-rhv, _elem[1]-rhv) ;
            }

        public: // mult

            inline void mult( type_t value ){
                _elem[0]*=value; _elem[1]*=value; _elem[2]*=value;
            }

        public: // operator *

            //***************************************************
            this_cref_t operator *= ( this_cref_t rhv ){
                _elem[0]*=rhv.x(); _elem[1]*=rhv.y();
                return (*this) ;
            }

            //***************************************************
            this_t operator * ( this_cref_t rhv ) const{
                return this_t(_elem[0]*rhv.x(), _elem[1]*rhv.y()) ;
            }

            //***************************************************
            this_cref_t operator *= ( type_cref_t rhv ){
                _elem[0]*=rhv; _elem[1]*=rhv;
                return (*this) ;
            }

            //***************************************************
            this_t operator * ( type_cref_t rhv ) const{
                return this_t(_elem[0]*rhv, _elem[1]*rhv) ;
            }

            //***************************************************
            template< typename other_t >
            this_t operator * ( other_t rhv ) const{
                return this_t( type_t(_elem[0]*rhv), type_t(_elem[1]*rhv) ) ;
            }

            //***************************************************
            template< typename other_t >
            this_t operator * ( vector2<other_t> rhv ) const{
                return this_t( type_t(_elem[0]*rhv.x()), type_t(_elem[1]*rhv.y()) ) ;
            }

        public: // operator /

            //***************************************************
            this_ref_t operator /= ( this_cref_t rhv ){
                _elem[0]/=rhv.x(); _elem[1]/=rhv.y(); 
                return (*this) ;
            }

            //***************************************************
            this_t operator / ( this_cref_t rhv ) const{
                return this_t(_elem[0]/rhv.x(), _elem[1]/rhv.y()) ;
            }

            //***************************************************
            this_ref_t operator /= ( type_cref_t rhv ){
                _elem[0]/=rhv; _elem[1]/=rhv; 
                return (*this) ;
            }

            //***************************************************
            this_t operator / ( type_cref_t rhv ) const{
                return this_t(_elem[0]/rhv, _elem[1]/rhv) ;
            }

        public: // operator %

            //***************************************************
            this_t operator % ( this_cref_t rhv ) const{
                return this_t(_elem[0]%rhv.x(), _elem[1]%rhv.y()) ;
            }

        public: // operator =

            //***************************************************
            this_ref_t operator = ( this_cref_t rhv ){
                _elem[0] = rhv.x() ; _elem[1] = rhv.y() ; 
                return (*this) ;
            }

            //***************************************************
            this_ref_t operator = ( vec4_t const & rhv ){
                _elem[0] = rhv.x() ; _elem[1] = rhv.y() ; 
                return (*this) ;
            }

        public: // bitwise

            this_t operator &( this_cref_t rhv ) const
            {
                return this_t( _elem[0]&rhv.x(), _elem[1]&rhv.y() ) ;
            }

            this_t operator |( this_cref_t rhv ) const
            {
                return this_t( _elem[0]|rhv.x(), _elem[1]|rhv.y() ) ;
            }
        
            this_t operator <<( this_cref_t rhv ) const 
            {
                return this_t( _elem[0]<<rhv.x(), _elem[1]<<rhv.y() ) ;
            }

            this_t operator >>( this_cref_t rhv ) const
            {
                return this_t( _elem[0]>>rhv.x(), _elem[1]>>rhv.y() ) ;
            }

        public: // relational 

            /// this < rhv
            vecb_t less_than( this_cref_t rhv ) const
            {
                return vecb_t( _elem[0] < rhv.x(), _elem[1] < rhv.y() ) ;
            }

            /// this <= rhv
            vecb_t less_equal_than( this_cref_t rhv ) const
            {
                return vecb_t( _elem[0] <= rhv.x(), _elem[1] <= rhv.y() ) ;
            }

            /// this > rhv
            vecb_t greater_than( this_cref_t rhv ) const
            {
                return vecb_t( _elem[0] > rhv.x(), _elem[1] > rhv.y() ) ;
            }

            /// this >= rhv
            vecb_t greater_equal_than( this_cref_t rhv ) const
            {
                return vecb_t( _elem[0] >= rhv.x(), _elem[1] >= rhv.y() ) ;
            }

            /// this == rhv integer
            vecb_t equal( this_cref_t rhv ) const
            {
                return vecb_t( _elem[ 0 ] == rhv.x(), _elem[ 1 ] == rhv.y() ) ;
            }

        public: // min/max

            /// component-wise max operation
            /// returns max of this or rhv.
            this_ref_t max( this_cref_t rhv ) 
            {
                _elem[0] = rhv._elem[0] > _elem[0] ? rhv._elem[0] : _elem[0] ;
                _elem[1] = rhv._elem[1] > _elem[1] ? rhv._elem[1] : _elem[1] ;
                return *this ;
            }

            this_t max_ed( this_cref_t rhv ) const 
            {
                return this_t(this_t(*this).max( rhv )) ;
            }

            /// component-wise min operation
            this_ref_t min( this_cref_t rhv ) 
            {
                _elem[0] = rhv._elem[0] < _elem[0] ? rhv._elem[0] : _elem[0] ;
                _elem[1] = rhv._elem[1] < _elem[1] ? rhv._elem[1] : _elem[1] ;
                return *this ;
            }

            this_t min_ed( this_cref_t rhv ) const 
            {
                return this_t(this_t(*this).min( rhv )) ;
            }

        public: // common

            //***************************************************
            type_t dot( this_cref_t rhv ) const{
                return _elem[0]*rhv.x()+_elem[1]*rhv.y();
            }

            //***************************************************
            type_t det( this_cref_t rhv ) const
            {
                return _elem[0] * rhv._elem[1] - _elem[1] * rhv._elem[0] ;
            }

            //***************************************************
            this_ref_t normalize( void ){
                type_t len = this_t::length() ;
                if( std::abs(len) > std::numeric_limits<type_t>::epsilon() ) (*this) /= len ;
                return (*this) ;
            }

            //***************************************************
            this_t normalized( void ) const {
                this_t vec( *this ) ;
                vec.normalize() ;
                return vec ;
            } 
        
            //***************************************************
            type_t length2( void ) const {
                return _elem[0]*_elem[0]+_elem[1]*_elem[1] ;
            }

            //***************************************************
            type_t length( void ) const {
                return sqrt(this_t::length2()) ;
            }

            //***************************************************
            this_ref_t negate( void )
            {
                _elem[0] = -_elem[0] ;_elem[1] = -_elem[1] ;
                return ( *this ) ;
            }

            //***************************************************
            this_t negated( void ) const
            {
                this_t v(*this) ;
                return v.negate() ;
            }

            //***************************************************
            this_t sign( void ) const{
                return this_t( natus::math::fn<type_t>::sign(_elem[0]), 
                    natus::math::fn<type_t>::sign(_elem[1]) ) ;
            }

            //***************************************************
            this_ref_t abs( void )
            {
                _elem[0] = type_t(std::abs(_elem[0])) ;
                _elem[1] = natus::math::fn<type_t>::abs(_elem[1]) ;
                return *this ;
            }

            //***************************************************
            this_t absed( void ) const
            {
                return this_t(*this).abs() ;
            }

            /// dont use
            /// sets the values of this to tyep_t(0) if the values are lower
            /// than the dead zone threshold.
            /// @return operates on this object. returns this.
            this_ref_t dead_zone( type_t threshold )
            {
                _elem[0] = std::abs(_elem[0]) < threshold ? type_t(0) : _elem[0]  ;
                _elem[1] = std::abs(_elem[1]) < threshold ? type_t(0) : _elem[1]  ;
                return *this ;
            }

            /// dont use
            /// sets the values of this to tyep_t(0) if the values are lower
            /// than the dead zone threshold.
            /// @return returns a new object.
            this_t dead_zoned( type_t threshold ) const
            {
                return this_t(*this).dead_zone( threshold ) ;
            }

            //***************************************************
            this_ref_t clamp( this_cref_t min_val, this_cref_t max_val ) 
            {
                _elem[0] = std::min( std::max(_elem[0], min_val.x()), max_val.x()) ;
                _elem[1] = std::min( std::max(_elem[1], min_val.y()), max_val.y()) ;

                return *this ;
            }

            //***************************************************
            this_t clamped( this_cref_t min_val, this_cref_t max_val ) const
            {
                return this_t(*this).clamp( min_val, max_val ) ;
            }

            //***************************************************
            this_ref_t clamp( type_t min_val, type_t max_val ) 
            {
                _elem[0] = std::min( std::max(_elem[0], min_val), max_val) ;
                _elem[1] = std::min( std::max(_elem[1], min_val), max_val) ;

                return *this ;
            }

            //***************************************************
            this_t clamped( type_t min_val, type_t max_val ) const 
            {
                return this_t(*this).clamp( min_val, max_val ) ;
            }

            //***************************************************
            type_t cross_as_scalar( this_cref_t other ) const 
            {
                return this_t::x() * other.y() - this_t::y() * other.x() ;
            }

            //***************************************************
            vec3_t crossed( this_cref_t other ) const 
            {
                return vec3_t( type_t(0), type_t(0), this_t::cross_as_scalar(other) ) ;
            }

            //***************************************************
            vec2_ref_t fract( void_t )
            {
                _elem[ 0 ] = natus::math::fn<type_t>::fract( _elem[ 0 ] ) ;
                _elem[ 1 ] = natus::math::fn<type_t>::fract( _elem[ 1 ] ) ;

                return *this ;
            }

            //***************************************************
            vec2_t fracted( void_t ) const
            {
                return this_t(
                    natus::math::fn<type_t>::fract( _elem[ 0 ] ),
                    natus::math::fn<type_t>::fract( _elem[ 1 ] ) );
            }

            //***************************************************
            vec2_ref_t floor( void_t ) 
            {
                _elem[ 0 ] = std::floor( _elem[ 0 ] ) ;
                _elem[ 1 ] = std::floor( _elem[ 1 ] ) ;
                return *this ;
            }

            //***************************************************
            vec2_t floored( void_t ) const
            {
                return this_t(
                    std::floor( _elem[ 0 ] ),
                    std::floor( _elem[ 1 ] ) ) ;
            }

        private:

                type_t _elem[2] ;

        } ;

        natus_typedefs( vector2< char_t >, vec2c ) ;
        natus_typedefs( vector2< uchar_t >, vec2uc ) ;
        natus_typedefs( vector2< int_t >, vec2i ) ;
        natus_typedefs( vector2< int16_t >, vec2i16 ) ;
        natus_typedefs( vector2< float_t >, vec2f ) ;
        natus_typedefs( vector2< double_t >, vec2d ) ;
        natus_typedefs( vector2< uint_t >, vec2ui ) ;
        natus_typedefs( vector2< ushort_t >, vec2us ) ;
    }
}