#pragma once

#include "typedefs.h"
#include "macros/handle.h"

#include <natus/log/global.h>
#include <natus/core/macros/move.h>

#include <mutex>
#include <type_traits>
#include <memory>
 
namespace natus
{
    namespace memory
    {
        natus_class_proto_typedefs( shared_data ) ;

        template< typename T >
        class handle
        {
            natus_this_typedefs( handle< T > ) ;
            natus_typedefs( T, type ) ;

            template< typename U >
            friend class handle ;

        private:

            type_ptr_t _ptr = nullptr ;
            shared_data_ptr_t _sd = nullptr ;

        public:

            handle( void_t )
            {
                _sd = natus::memory::global::alloc( shared_data_t(),
                    "[natus::memory::handle] : shared_data" ) ;
            }

            handle( type_rref_t v )
            {
                _sd = natus::memory::global::alloc( shared_data_t(),
                    "[natus::memory::handle] : shared_data" ) ;

                _ptr = natus::memory::global::alloc( ::std::move( v ),
                    "[natus::memory::handle::handle] : allocated via handle" ) ;
            }

            handle( type_rref_t v, natus::memory::purpose_cref_t p )
            {
                _sd = natus::memory::global::alloc( shared_data_t(),
                    "[natus::memory::handle] : shared_data" ) ;

                _ptr = natus::memory::global::alloc( ::std::move( v ), p ) ;
            }

            handle( type_ptr_t ptr ) : _ptr( ptr )
            {
                _sd = natus::memory::global::alloc( shared_data_t(),
                    "[natus::memory::handle] : shared_data" ) ;
            }

            handle( this_cref_t rhv ) : _ptr( rhv._ptr )
            {
                _sd = rhv._sd ;
                _sd->increment() ;
            }

            handle( this_rref_t rhv )
            {
                natus_move_member_ptr( _ptr, rhv ) ;
                natus_move_member_ptr( _sd, rhv ) ;
            }

            template< typename C >
            handle( handle<C> const& rhv )
            {
                _ptr = do_cast< C, T >::now( const_cast< C* >( rhv.get() ) ) ;

                _sd = const_cast< handle<C>& >( rhv )._sd ;
                _sd->increment() ;
            }

            template< typename C >
            handle( handle<C>&& rhv )
            {
                _ptr = do_cast< C, T >::now( rhv._ptr ) ;
                rhv._ptr = nullptr ;

                natus_move_member_ptr( _sd, rhv ) ;
            }

            this_ref_t operator = ( this_cref_t rhv )
            {
                this_t::release() ;

                _ptr = rhv._ptr ;
                _sd = rhv._sd ;
                _sd->increment() ;

                return *this ;
            }

            this_ref_t operator = ( this_rref_t rhv )
            {
                this_t::release() ;

                natus_move_member_ptr( _ptr, rhv ) ;
                natus_move_member_ptr( _sd, rhv ) ;

                return *this ;
            }

            template< typename C >
            this_ref_t operator = ( handle<C> const& rhv )
            {
                this_t::release() ;

                _ptr = rhv._ptr ;
                _sd = rhv._sd ;
                _sd->increment() ;

                return *this ;
            }

            template< typename C >
            this_ref_t operator = ( handle<C>&& rhv )
            {
                this_t::release() ;

                _ptr = do_cast< C, T >::now( rhv._ptr ) ;
                rhv._ptr = nullptr ;

                natus_move_member_ptr( _ptr, rhv ) ;
                natus_move_member_ptr( _sd, rhv ) ;

                return *this ;
            }

            /// at the moment this class is like a pointer,
            /// so we can not call a deallocation
            ~handle( void )
            {
                this_t::release() ;
            }

        public:

            T const* get( void_t ) const { return _ptr; }
            T* get( void_t ) { return _ptr ; }

        public:

            void_t release( void_t )
            {
                if( natus::core::is_not_nullptr( _sd ) && _sd->decrement() == 0 )
                {
                    natus::memory::global::dealloc( _sd ) ;
                    natus::memory::global::dealloc( _ptr ) ;

                    _sd = nullptr ;
                    _ptr = nullptr ;
                }
            }

        public:

            T* operator ->( void_t ) {
                return _ptr ;
            }


        };

        template< typename From, typename To,
            // read: Up=To is base of From; Down=From is base of To
            bool Up = ::std::is_base_of<To, From>::value, bool Down = ::std::is_base_of<From, To>::value >
            struct do_cast
        {
            static To* now( From* from ) ;
        };

        template< typename From, typename To >
        struct do_cast< From, To, false, false >
        {
            static To* now( From* from )
            {
                static_assert( false, "no inheritance between types" ) ;
            }
        };

        template< typename From, typename To >
        struct do_cast< From, To, true, true >
        {};

        // up
        template< typename From, typename To >
        struct do_cast< From, To, true, false >
        {
            static To* now( From* from )
            {
                return from ;
            }
        };

        // down
        template< typename From, typename To >
        struct do_cast< From, To, false, true >
        {
            static To* now( From* from )
            {
                return dynamic_cast< To* >( from ) ;
            }
        };

        class shared_data
        {
            natus_this_typedefs( shared_data ) ;

        private:

            ::std::mutex _mtx ;
            size_t _ref_count = 1 ;

        public:

            shared_data( void_t ) {}

            shared_data( this_cref_t rhv ) = delete ;

            shared_data( this_rref_t rhv )
            {
                _ref_count = rhv._ref_count ;
                rhv._ref_count = 0 ;
            }

            ~shared_data( void_t )
            {
                natus::log::global::error_and_exit( _ref_count != 0,
                    "[natus::memory::handle::~shared_data] : ref_count != 0" ) ;
            }

            this_ref_t operator = ( this_cref_t rhv )
            {
                _ref_count = rhv._ref_count ;
                return *this ;
            }

            this_ref_t operator = ( this_rref_t rhv )
            {
                _ref_count = rhv._ref_count ;
                return *this ;
            }

        public:

            size_t increment( void_t )
            {
                ::std::lock_guard<std::mutex> lk( _mtx ) ;
                return ++_ref_count ;
            }

            size_t decrement( void_t )
            {
                ::std::lock_guard<std::mutex> lk( _mtx ) ;
                natus::log::global::error_and_exit( _ref_count == 0,
                    "[natus::memory::handle::shared_data::decrement] : ref_count == 0" ) ;
                return --_ref_count ;
            }
        };
        natus_typedef( shared_data ) ;
    }
}