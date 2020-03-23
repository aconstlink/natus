

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
            natus_this_typedefs( rptr< T > ) ;
            natus_typedefs( T, value ) ;

            natus::soil::res_t _res ;

        public:

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
            
        public:

            value_ptr_t operator -> ( void_t ) 
            {
                return _res.cast< value_ptr_t >() ;
            }

            value_cptr_t operator -> ( void_t ) const
            {
                return _res.cast< value_cptr_t >() ;
            }

        public:

            natus::soil::res_t res( void_t ) const { return _res ; }
        };
    }
}