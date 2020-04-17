

#pragma once

#include "res.hpp"

#include <exception>

namespace natus
{
    namespace soil
    {
        // res ptr for any type is not allowed/implemented.
        template< class T >
        class rptr
        {};

        // this seems to work for T const * and T *
        template< class T >
        class rptr< T* >
        {
            natus_this_typedefs( rptr< T* > ) ;
            natus_typedefs( T, value ) ;

            natus::soil::res_t _res ;

        public:

            template< class O >
            rptr( rptr< O* > rhv ) : rptr(rhv.res())
            {
            }

            rptr( void_t ) noexcept
            {}

            rptr( natus::soil::res_t res  ) 
            {
                if( natus::core::is_not( res.is_castable<value_ptr_t>() ) )
                    throw ::std::invalid_argument( "[] : Can not cast res to template type" ) ;
                _res = res ;
            }

            rptr( this_cref_t rhv ) 
            {
                _res = rhv._res ;
            }

            rptr( this_rref_t rhv ) 
            {
                _res = ::std::move( rhv._res ) ;
            }

            ~rptr( void_t ) {}
            
            bool_t is_valid( void_t ) const
            {
                return _res.is_valid() ;
            }
            bool_t is_not_valid( void_t ) const
            {
                return natus::core::is_not( _res.is_valid() ) ;
            }


        public:

            value_ptr_t operator -> ( void_t ) 
            {
                return _res.cast< value_ptr_t >() ;
            }

            value_cptr_t operator -> ( void_t ) const
            {
                return _res.cast< value_cptr_t >() ;
            }

            this_ref_t operator = ( this_rref_t rhv ) 
            {
                _res = ::std::move( rhv._res ) ;
                return *this ;
            }

            this_ref_t operator = ( this_cref_t rhv )
            {
                _res = rhv._res ;
                return *this ;
            }

            bool_t operator == ( this_cref_t rhv ) const noexcept
            {
                return _res == rhv._res ;
            }

            bool_t equals ( this_cref_t rhv ) const noexcept
            {
                return _res == rhv._res ;
            }

        public:

            natus::soil::res_t res( void_t ) const { return _res ; }
        };
    }
}