#pragma once

#include "api.h"
#include "typedefs.h"

#include <natus/memory/global.h>
#include <natus/log/global.h>
#include <natus/std/vector.hpp>
#include <natus/std/string.hpp>

#include <functional>
#include <exception>
#include <atomic>
#include <memory>
#include <type_traits>

namespace natus
{
    namespace soil
    {
        template< class T >
        class res 
        {
            natus_this_typedefs( res<T> ) ;
            natus_typedefs( T, value ) ;

        private:

            typedef ::std::shared_ptr< T > sptr_t ;
            typedef ::std::shared_ptr< T > const scptr_t ;
            typedef ::std::shared_ptr< const T > sptrc_t ;

            sptr_t _data ;

        public:

            sptr_t get_sptr( void_t ) { return _data ;  }
            sptr_t get_sptr( void_t ) const { return _data ;  }

            sptr_t move_out_sptr( void_t ) { return ::std::move( _data ) ; }

        public:

            res( void_t )
            {}

            res( this_cref_t rhv )
            {
                _data = rhv._data ;
            }

            res( this_rref_t rhv )
            {
                _data = ::std::move( rhv._data ) ;
            }

            template< class B >
            res( res< B > const & rhv )
            {
                _data = rhv.get_sptr() ;
            }

            template< class B >
            res( res< B > && rhv )
            {
                _data = ::std::move( rhv.move_out_sptr() ) ;
            }
            
            res( value_cref_t v )
            {
                _data = sptr_t( natus::memory::global_t::alloc<T>( v ),
                    [=] ( T* ptr )
                {
                    natus::memory::global_t::dealloc( ptr ) ;
                } ) ;
            }

            res( value_rref_t v )
            {
                _data = sptr_t( natus::memory::global_t::alloc<T>( ::std::move(v) ),
                    [=] ( T* ptr )
                {
                    natus::memory::global_t::dealloc( ptr ) ;
                } ) ;
            }

            ~res( void_t ) 
            {
            }

        public:

            sptr_t operator -> ( void_t ) 
            {
                return _data ;
            }

            scptr_t operator -> ( void_t ) const
            {
                return _data ;
            }

            T & operator * ( void_t )
            {
                return *_data ;
            }

            T const & operator * ( void_t ) const
            {
                return *_data ;
            }

            this_ref_t operator = ( this_cref_t rhv )
            {
                _data = rhv._data ;
                return *this ;
            }

            this_ref_t operator = ( this_rref_t rhv )
            {
                _data = ::std::move( rhv._data ) ;
                return *this ;
            }

            this_ref_t operator = ( value_rref_t v )
            {
                if( _data == nullptr ) 
                {
                    _data = sptr_t( natus::memory::global_t::alloc<T>( ::std::move( v ) ),
                        [=] ( T* ptr )
                    {
                        natus::memory::global_t::dealloc( ptr ) ;
                    } ) ;
                }
                else
                {
                    *_data = ::std::move( v ) ;
                }
                
                return *this ;
            }

            template< class B >
            this_ref_t operator = ( res<B> const & rhv )
            {
                _data = ::std::dynamic_pointer_cast<T>( rhv.get_sptr() ) ;
                return *this ;
            }

            template< class B >
            this_ref_t operator = ( res<B> && rhv )
            {
                _data = ::std::move( rhv.move_out_sptr() ) ;
                return *this ;
            }

        public:

            bool_t is_valid( void_t ) const { return _data != nullptr ; }

        public:

            template< typename B >
            static bool_t castable( B const & o )
            {
                return dynamic_cast< value_ptr_t >( o.get_sptr().get() ) != nullptr ;
            }
        };
    }
}