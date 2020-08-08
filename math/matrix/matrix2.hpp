#pragma once

#include "protos.hpp"

#include "../typedefs.h"
#include "../vector/protos.hpp"

#include "switch.h"

namespace natus
{
    namespace math
    {
        template< typename type >
        class matrix2
        {
        public: // typedefs 

            typedef type type_t ;
            typedef type_t & type_ref_t ;

            typedef matrix2< type_t > this_t ;
            typedef matrix3< type_t > mat3_t ;
            typedef matrix4< type_t > mat4_t ;

            typedef vector2< type_t > vec2_t ;
            typedef vec2_t const & vec2_cref_t ;

            typedef vector3< type_t > vec3_t ;
            typedef vector4< type_t > vec4_t ;

            typedef this_t & this_ref_t ;
            typedef this_t && this_rref_t ;
            typedef this_t const & this_cref_t ;

        public: // ctor

            //************************************************************************************
            matrix2( void ) 
            {
                for( size_t i = 0; i<4; ++i )
                    _elem[i] = type_t(0) ;
            }

            //************************************************************************************
            matrix2( this_cref_t rhv ) 
            {
                for( size_t i=0; i<4; ++i ){
                    _elem[i] = rhv[i] ;
                }
            }

            matrix2( this_rref_t rhv ) noexcept
            {
                for( size_t i=0; i<4; ++i ){
                    _elem[i] = rhv[i] ;
                }
            }

            this_ref_t operator = ( this_cref_t rhv ) noexcept
            {
                for( size_t i=0; i<4; ++i ){
                    _elem[i] = rhv[i] ;
                }
            }

            this_ref_t operator = ( this_rref_t rhv ) noexcept
            {
                for( size_t i=0; i<4; ++i ){
                    _elem[i] = rhv[i] ;
                }
                return *this ;
            }

            //************************************************************************************
            matrix2( type_t s ) 
            {
                for( size_t i=0; i<4; ++i ){
                    _elem[i] = s ;
                }
            }

            //************************************************************************************
            matrix2( mat4_t const & rhv )
            {
                set_column( 0, rhv.column3(0) ) ;
                set_column( 1, rhv.column3(1) ) ;
            }
    
            //************************************************************************************
            matrix2( type_t cos_a, type_t sin_a, natus::math::rotation_matrix )
            {
                set_row( 0, vec2_t( cos_a, -sin_a ) ) ;
                set_row( 1, vec2_t( sin_a, cos_a ) ) ;
            }

            //************************************************************************************
            matrix2( vec2_cref_t cos_sin, natus::math::rotation_matrix )
            {
                set_row( 0, vec2_t( cos_sin.x(), -cos_sin.y() ) ) ;
                set_row( 1, vec2_t( cos_sin.y(), cos_sin.x() ) ) ;
            }

        public: // operator []

            //************************************************************************************
            type_t operator[]( size_t index ) const {
                return _elem[index] ;
            }

            //************************************************************************************
            type_ref_t operator[]( size_t index ){
                return _elem[index] ;
            }

        public: // operator ()

            //************************************************************************************
            this_cref_t operator()( type_t rhv ){
                for( size_t i=0; i<4; ++i ) _elem[i] = rhv ;
                return (*this) ;
            }

            //************************************************************************************
            type_t operator()( size_t row, size_t column ) const {
                return _elem[(row<<1)+column] ;
            }

        public: // common

            //************************************************************************************
            vec2_t row( size_t i ) const 
            {
                const size_t start = i<<1 ;
                return vec3_t( _elem[start],_elem[start+1], _elem[start+2] ) ;
            }

            //************************************************************************************
            this_ref_t set_row( size_t i, vec4_t const & v )
            {
                const size_t start = i<<1 ;
                _elem[start] = v.x() ;
                _elem[start+1] = v.y() ;
                return *this ;
            }

            //************************************************************************************
            this_ref_t set_row( size_t i, vec3_t const & v )
            {
                const size_t start = i<<1 ;
                _elem[start] = v.x() ;
                _elem[start+1] = v.y() ;
                return *this ;
            }

            //************************************************************************************
            this_ref_t set_row( size_t i, vec2_t const & v )
            {
                const size_t start = i<<1 ;
                _elem[start] = v.x() ;
                _elem[start+1] = v.y() ;
                return *this ;
            }

            //************************************************************************************
            vec2_t column( size_t i ) const 
            { 
                return vec2_t( _elem[i], _elem[i+2] ) ; 
            }

            //************************************************************************************
            vec2_t get_column( size_t i ) const 
            { 
                return column(i) ; 
            }

            //************************************************************************************
            this_ref_t set_column( size_t i, vec2_t const & v ) 
            {
                _elem[i] = v.x();
                _elem[i+2] = v.y();
                return *this ;
            }

            //************************************************************************************
            this_ref_t zero( void_t )
            {
                for(size_t i=0;i<4; ++i) _elem[i] = type_t(0);
                return *this ;
            }

            //************************************************************************************
            this_ref_t transpose( void )
            {
                type_t tmp = _elem[2] ;
                _elem[2] = _elem[1] ;
                _elem[1] = tmp ;

                return *this ;
            }

            //************************************************************************************
            this_t transposed( void ) const
            {
                this_t mat( *this ) ;
                return mat.transpose() ;
            }

            //************************************************************************************
            this_ref_t identity( void )
            {
                this->zero() ;
                _elem[0] = type_t(1);
                _elem[3] = type_t(1);
                return (*this) ;
            }

            //************************************************************************************
            this_t identitied( void ) const 
            {
                this_t mat( *this ) ;
                return mat.identity() ;
            }

            //************************************************************************************
            this_ref_t scale( type_t rhv )
            {
                _elem[0] *= rhv ;
                _elem[3] *= rhv ;
                return (*this) ;	
            }

            //************************************************************************************
            this_t scaled( type_t rhv ) const
            {
                this_t mat( *this ) ;
                return mat.scale( rhv ) ;
            }

            //************************************************************************************
            this_ref_t scale( type_t sx, type_t sy )
            {
                _elem[0] *= sx ;
                _elem[3] *= sy ;
                return (*this) ;	
            }

            //************************************************************************************
            this_t scaled( type_t sx, type_t sy ) const 
            {
                this_t mat( *this ) ;
                return mat.scale( sx, sy ) ;
            }

            //************************************************************************************
            this_ref_t scale( vec3_t const & rhv )
            {
                return this_t::scale( rhv.x(), rhv.y() ) ;	
            }

            //************************************************************************************
            this_t scaled( vec3_t const & rhv ) const 
            {
                return this_t::scaled( rhv.x(), rhv.y() ) ;	
            }

            //************************************************************************************
            type_t trace( void ) const 
            {
                return _elem[0] + _elem[3] ;
            }

            //************************************************************************************
            type_t angle( void ) const 
            {
                return ::std::acos( (this_t::trace()-type_t(2))*(type_t(0.5)) ) ;
            }

        public: // operator +

            //************************************************************************************
            this_t operator + ( type_t rhv ) const 
            {
                this_t mat((*this));
                for( size_t i=0; i<4; ++i ) mat[i] += rhv ;
                return mat ;
            }

            //************************************************************************************
            this_t operator + ( this_cref_t rhv ) const 
            {
                this_t mat ;
                for( size_t i=0; i<4; ++i ) mat[i] = _elem[i] + rhv[i] ; 
                return mat ;
            }

            //************************************************************************************
            this_ref_t operator += ( type_t rhv ) 
            {
                for( size_t i=0; i<4; ++i ) _elem[i] += rhv ;
                return (*this) ;
            }

            //************************************************************************************
            this_ref_t operator += ( this_cref_t rhv )
            {
                for( size_t i=0; i<4; ++i ) _elem[i] += rhv[i] ; 
                return *this ;
            }

        public: // operator -

            //************************************************************************************
            this_t operator - ( type_t rhv ) const 
            {
                this_t mat( *this ) ;
                for( size_t i=0; i<4; ++i ) mat[i] -= rhv ;
                return mat ;
            }


            //************************************************************************************
            this_ref_t operator -= ( type_t rhv ) 
            {
                for( size_t i=0; i<4; ++i ) _elem[i] -= rhv ;
                return *this ;
            }

            //************************************************************************************
            this_ref_t operator -= ( this_cref_t rhv ) 
            {
                for( size_t i=0; i<4; ++i ) _elem[i] -= rhv[i] ; 
                return *this ;
            }

        public: // operator *

            //************************************************************************************
            this_t operator * ( type_t rhv ) const 
            {
                this_t mat( *this );
                for( size_t i=0; i<4; ++i ) mat[i] *= rhv ;
                return mat ;
            }

            //************************************************************************************
            this_cref_t operator *= ( type_t rhv ) 
            {
                for( size_t i=0; i<4; ++i ) _elem[i] *= rhv ;
                return (*this) ;
            }

            //************************************************************************************
            vec2_t operator * ( vec2_t const & rhv ) const 
            {
                vec2_t vec ;
                for( size_t i=0; i<2; ++i ){
                    vec[i] = row(i).dot( rhv ) ;
                }
                return vec ;
            }

            //************************************************************************************
            this_t operator * ( this_cref_t rhv ) const 
            {
                this_t mat ;
            
                mat[0] = this_t::row(0).dot(rhv.column(0)) ;
                mat[1] = this_t::row(0).dot(rhv.column(1)) ;
            
                mat[2] = this_t::row(1).dot(rhv.column(0)) ;
                mat[3] = this_t::row(1).dot(rhv.column(1)) ;
            

                return mat ;
            } 

            //************************************************************************************
            this_ref_t operator *= ( this_cref_t rhv ) 
            {
                this_t mat( *this ) ;

                _elem[0] = mat.row(0).dot(rhv.column(0)) ;
                _elem[1] = mat.row(0).dot(rhv.column(1)) ;

                _elem[2] = mat.row(1).dot(rhv.column(0)) ;
                _elem[3] = mat.row(1).dot(rhv.column(1)) ;

                return *this ;
            }

        private:

            /// the matrix elements.
            type_t _elem[4] ;

        } ;
    
        typedef matrix2< int_t > mat2i_t ;
        typedef matrix2< float_t > mat2f_t ;
        typedef matrix2< double_t > mat2d_t ;
        typedef matrix2< uint_t > mat2ui_t ;
        typedef matrix2< size_t > mat2s_t ;
    }
}
