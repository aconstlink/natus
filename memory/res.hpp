#pragma once

#include "api.h"
#include "typedefs.h"
#include "global.h"

#include <natus/log/global.h>

#include <functional>
#include <exception>
#include <atomic>
#include <memory>
#include <type_traits>

namespace natus
{
    namespace memory
    {
        template< class T >
        class res 
        {
            typedef res<T> _this_t ;
            natus_this_typedefs( _this_t ) ;
            natus_typedefs( T, value ) ;

        private:

            typedef std::shared_ptr< T > sptr_t ;
            typedef std::shared_ptr< T > const scptr_t ;
            typedef std::shared_ptr< const T > sptrc_t ;

            sptr_t _data ;

        public:

            sptr_t get_sptr( void_t ) { return _data ;  }
            sptr_t get_sptr( void_t ) const { return _data ;  }

            sptr_t move_out_sptr( void_t ) { return std::move( _data ) ; }

        public:

            res( void_t ) noexcept
            {}

            res( this_cref_t rhv ) noexcept
            {
                _data = rhv._data ;
            }

            res( this_rref_t rhv ) noexcept
            {
                _data = std::move( rhv._data ) ;
            }

            template< class B >
            res( res< B > const & rhv ) noexcept
            {
                _data = std::dynamic_pointer_cast<T>( rhv.get_sptr() ) ;
            }

            template< class B >
            res( res< B > && rhv )
            {
                _data = std::dynamic_pointer_cast<T>( std::move( rhv.move_out_sptr() ) ) ;
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

            template< class T, class D >
            res( T * ptr, D deleter ) noexcept
            {
                _data = sptr_t( ptr, deleter ) ;
            }

            ~res( void_t ) 
            {
            }

        public:

            sptr_t operator -> ( void_t ) noexcept
            {
                return _data ;
            }

            scptr_t operator -> ( void_t ) const noexcept
            {
                return _data ;
            }

            T & operator * ( void_t ) noexcept
            {
                return *_data ;
            }

            T const & operator * ( void_t ) const noexcept
            {
                return *_data ;
            }

            this_ref_t operator = ( this_cref_t rhv ) noexcept
            {
                _data = rhv._data ;
                return *this ;
            }

            this_ref_t operator = ( this_rref_t rhv ) noexcept
            {
                _data = ::std::move( rhv._data ) ;
                return *this ;
            }

            this_ref_t operator = ( value_rref_t v ) noexcept
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
                    *_data = std::move( v ) ;
                }
                
                return *this ;
            }

            template< class B >
            this_ref_t operator = ( res<B> const & rhv ) noexcept
            {
                _data = std::dynamic_pointer_cast<T>( rhv.get_sptr() ) ;
                return *this ;
            }

            template< class B >
            this_ref_t operator = ( res<B> && rhv ) noexcept
            {
                _data = std::move( rhv.move_out_sptr() ) ;
                return *this ;
            }

            bool_t operator == ( this_cref_t rhv ) const noexcept
            {
                return _data == rhv._data ;
            }

        public:

            bool_t is_valid( void_t ) const noexcept { return _data != nullptr ; }

        public:

            template< typename B >
            static bool_t castable( B const & o ) noexcept
            {
                return dynamic_cast< value_ptr_t >( o.get_sptr().get() ) != nullptr ;
            }
        };

        template< class T >
        using res_t = res< T > ;

    }
}