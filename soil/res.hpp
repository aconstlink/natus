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
        namespace detail
        {
            template< class T >
            struct tag ;

            template<>
            struct tag< void_t >
            {
                virtual ~tag( void_t ){}
            };
            natus_typedefs( tag<void_t>, tag ) ;
        
            template< class T >
            struct tag : public tag_t, virtual T
            {
                virtual ~tag( void_t ) {}
            };
        }

        template< class T >
        class res ;
        
        template<>
        class res< void_t >
        {
            natus_this_typedefs( res ) ;

        protected: // struct

            struct ishared_data
            {
                natus_this_typedefs( ishared_data ) ;

                ::std::atomic<size_t> _ref_count ;

                ishared_data( void_t ) noexcept : _ref_count(0) {}
                ishared_data( this_cref_t rhv ) noexcept : _ref_count( (size_t)rhv._ref_count ) 
                {}

                ishared_data( this_rref_t rhv ) noexcept : _ref_count((size_t)rhv._ref_count)
                {}

                virtual ~ishared_data( void_t )
                {}

                void_t aquire( void_t ) noexcept
                {
                    ++_ref_count ;
                    natus::log::global_t::status( "[aquire] : " +
                        ::std::to_string( _ref_count ) ) ;
                }

                bool_t release( void_t ) noexcept
                {
                    --_ref_count ;
                    natus::log::global_t::status( "[release] : " +
                        ::std::to_string( _ref_count ) ) ;
                    
                    return _ref_count == 0 ;
                }

                virtual void_ptr_t get_ptr( void_t ) = 0 ;
                virtual natus::soil::detail::tag_cptr_t get_tag( void_t ) const = 0 ;
            };
            natus_typedef( ishared_data ) ;

        private: // variables

            ishared_data_ptr_t _sd = nullptr ;

        protected:

            res( ishared_data_ptr_t ptr, bool_t const b = true ) : _sd( ptr )
            {
                if( b ) this_t::aquire() ;
            }

        public:

            res( void_t ) noexcept
            {}

            res( this_cref_t rhv ) noexcept
            {
                _sd = rhv._sd ;
                this_t::aquire() ;
            }

            res( this_rref_t rhv ) noexcept
            {
                natus_move_member_ptr( _sd, rhv ) ;
            }

            virtual ~res( void_t ) 
            {
                if( natus::core::is_not_nullptr( _sd ) &&
                    _sd->release() )
                {
                    natus::memory::global_t::dealloc( _sd ) ;
                }
            }

            this_ref_t operator = ( this_cref_t rhv ) noexcept
            {
                _sd = rhv._sd ;
                if( _sd ) _sd->aquire() ;
                return *this ;
            }

            this_ref_t operator = ( this_rref_t rhv ) noexcept
            {
                natus_move_member_ptr( _sd, rhv ) ;
                return *this ;
            }

            bool_t is_valid( void_t ) const noexcept
            {
                return natus::core::is_not_nullptr( _sd ) ;
            }

            void_t aquire( void_t ) noexcept
            {
                if( natus::core::is_not_nullptr( _sd ) )
                    _sd->aquire() ;
            }

        public:

            /// do not use this externally
            /// make const * const for protection
            ishared_data_t const * const get_shared_data( void_t ) const noexcept
            {
                return _sd ;
            }

            /// do not use this externally
            /// make const * const for protection
            ishared_data_t const* const move_shared_data( void_t ) const noexcept
            {
                auto* tmp = _sd ;
                const_cast<this_ptr_t>(this)->_sd = nullptr ;
                return tmp ;
            }

            /// unsafe cast of stored data
            template< class B >
            B cast( void_t )
            {
                static_assert( ::std::is_pointer<B>::value, "Must pass pointer type" ) ;

                return static_cast< B >( _sd->get_ptr() ) ;
            }

            /// unsafe cast of stored data
            template< class B >
            B cast( void_t ) const
            {
                static_assert( ::std::is_pointer<B>::value, "Must pass pointer type" ) ;

                return static_cast< const B >( _sd->get_ptr() ) ;
            }

            /// safe cast to pointer of B -> B*
            template< class B >
            B scast( void_t )
            {
                static_assert( ::std::is_pointer<B>::value, "Must pass pointer type" ) ;

                if( natus::core::is_not( this_t::is_valid() ) ) return nullptr ;
                if( auto* ptr = dynamic_cast< B >( const_cast<natus::soil::detail::tag_ptr_t>(_sd->get_tag() ) ) )
                {
                    return this_t::cast< B >() ;
                }
                return nullptr ;
            }

            template< class B >
            bool_t is_castable( void_t ) const 
            {
                static_assert( ::std::is_pointer<B>::value, "Must pass pointer type" ) ;

                auto * ptr = dynamic_cast< B >( const_cast< natus::soil::detail::tag_ptr_t >( _sd->get_tag() ) ) ;

                return natus::core::is_not_nullptr( ptr ) ;
            }

            bool_t operator == ( this_cref_t rhv ) const
            {
                if( natus::core::is_nullptr( _sd ) ) return false ;

                return _sd == rhv.get_shared_data() ;
            }

        protected:

            ishared_data_ptr_t get_( void_t ) 
            {
                return _sd ;
            }
        };
        natus_typedefs( res<void_t>, res ) ;

        //**********************************************
        template< class T >
        class res : public res_t
        {
            natus_this_typedefs( res<T> ) ;
            natus_typedefs( T, value ) ;

            friend res_t ;

        private:

            //**********************************************
            struct shared_data : public res_t::ishared_data
            {
                natus_this_typedefs( shared_data ) ;

            public:

                shared_data( void_t ) {}
                shared_data( value_ptr_t ptr ) : _pptr(ptr) {}
                shared_data( this_cref_t rhv ) : res_t::ishared_data_t( rhv ) 
                {
                    _pptr = rhv._pptr ;
                }

                shared_data( this_rref_t rhv ) : res_t::ishared_data_t( ::std::move( rhv ) )
                {
                    natus_move_member_ptr( _pptr, rhv ) ;
                }

                virtual ~shared_data( void_t ) {}

            public:

                value_ptr_t _pptr = nullptr ;

            private:

                virtual void_ptr_t get_ptr( void_t ) 
                {
                    return _pptr ;
                }

                virtual natus::soil::detail::tag_cptr_t get_tag( void_t ) const 
                {
                    static natus::soil::detail::tag< value_t > __tag__ ;
                    return &__tag__ ;
                }
            };
            natus_typedef( shared_data ) ;

        private:

            res( this_t::shared_data_ptr_t ptr, bool_t const b ) : res_t( ptr, b )
            {}

        public:

            res( value_ptr_t ptr ) : res_t( natus::memory::global_t::alloc< this_t::shared_data_t >(
                this_t::shared_data_t( ptr ), "[id] : shared_data" ) )
            {}

            res( void_t ) noexcept
            {
                natus::log::global_t::status( "[res(void)]" ) ;
            }

            res( this_cref_t rhv ) noexcept : res_t( rhv ) 
            {
                natus::log::global_t::status( "[res(cref)]" ) ;
            }

            res( this_rref_t rhv ) noexcept : res_t( ::std::move(rhv) ) 
            {
                natus::log::global_t::status( "[res(rref)]" ) ;
            }

            res( res_cref_t rhv ) noexcept
            {
                natus::log::global_t::status( "[res(res_cref)]" ) ;
                *this = rhv ;
            }

            res( res_rref_t rhv ) noexcept
            {
                natus::log::global_t::status( "[res(id_rref)]" ) ;
                *this = ::std::move( rhv ) ;
            }

            res( value_cref_t rhv ) noexcept
            {
                natus::log::global_t::status( "[res(value_cref)]" ) ;
                *this = rhv ;
            }

            res( value_rref_t rhv ) noexcept
            {
                natus::log::global_t::status( "[res(value_rref)]" ) ;
                *this = ::std::move( rhv ) ;
            }
            
            ~res( void_t ) noexcept
            {}
            
        public: // operator = 

            this_ref_t operator = ( res_cref_t rhv ) noexcept 
            {
                natus::log::global_t::status( "[res::operator =(res_cref)]" ) ;
                
                {
                    auto* ptr = dynamic_cast< this_t::shared_data_ptr_t >( const_cast< res_t::ishared_data_ptr_t >( rhv.get_shared_data() ) ) ;

                    if( natus::core::is_not_nullptr( ptr ) )
                    {
                        *this = this_t( ptr, true ) ;
                    }
                }

                return *this ;
            }

            this_ref_t operator = ( res_rref_t rhv ) noexcept
            {
                natus::log::global_t::status( "[res::operator =(id_rref)]" ) ;
                auto* ptr = dynamic_cast< this_t::shared_data_ptr_t >( const_cast< res_t::ishared_data_ptr_t >( rhv.get_shared_data() ) ) ;
                if( natus::core::is_not_nullptr( ptr ) )
                {
                    *this = this_t( static_cast<this_t::shared_data_ptr_t>(
                        const_cast< res_t::ishared_data_ptr_t >(rhv.move_shared_data())), false ) ;
                }

                return *this ;
            }

            this_ref_t operator = ( this_cref_t rhv ) noexcept
            {
                natus::log::global_t::status( "[res::operator =(cref)]" ) ;
                return static_cast<this_ref_t>( res_t::operator=( rhv ) ) ;
            }

            this_ref_t operator = ( this_rref_t rhv ) noexcept
            {
                natus::log::global_t::status( "[res::operator =(rref)]" ) ;
                return static_cast<this_ref_t>( res_t::operator=( ::std::move( rhv ) ) ) ;
            }

            this_ref_t operator = ( value_cref_t rhv ) noexcept
            {
                natus::log::global_t::status( "[res::operator =(value_cref)]" ) ;
                return *this = this_t::construct( rhv ) ;
            }
            
            this_ref_t operator = ( value_rref_t rhv ) noexcept
            {
                natus::log::global_t::status( "[res::operator =(value_rref)]" ) ;
                return *this = this_t::construct( ::std::move( rhv ) ) ;
            }
            
        public: // operator ->

            value_ptr_t operator -> ( void_t ) 
            {
                return static_cast< shared_data_ptr_t >( this_t::get_() )->_pptr ;
            }

        public: // construct 

            static this_t construct( value_cref_t t )
            {
                value_ptr_t ptr = natus::memory::global_t::alloc<value_t>( 
                    t, "[shared_data] : " + 
                    natus::std::string( typeid(value_t).name() ) ) ;
                                
                return this_t( ptr ) ;
            }

            static this_t construct( value_rref_t t )
            {
                value_ptr_t ptr = natus::memory::global_t::alloc<value_t>( 
                    ::std::move(t), "[shared_data] : " + 
                    natus::std::string_t( typeid(value_t).name() ) ) ;
                                               
                return this_t( ptr ) ;
            }
        };
    }
}