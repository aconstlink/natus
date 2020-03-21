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
#include <typeinfo>

namespace natus
{
    namespace soil
    {
        //**********************************************
        class id
        {
            natus_this_typedefs( id ) ;

        protected: // struct

            //**********************************************
            struct shared_data
            {
                natus_this_typedefs( shared_data ) ;

            public:

                shared_data( void_t ) {}
                shared_data( this_cref_t ) = delete ;
                shared_data( this_rref_t rhv ) : _ref_count((size_t)rhv._ref_count)
                {
                    _on_deletions = ::std::move( rhv._on_deletions ) ;
                }
                ~shared_data( void_t ) {}

                typedef ::std::function< void_t ( void_t ) > on_deletion_t ;

            private:

                ::std::mutex _mtx ;
                natus::std::vector< on_deletion_t > _on_deletions ;
                ::std::atomic<size_t> _ref_count = 0 ;

            public:

                /*void_t add( on_deletion_t funk ) noexcept
                {
                    try 
                    { 
                        ::std::lock_guard< ::std::mutex > lk( _mtx ) ;
                        _on_deletions.emplace_back( funk ) ; 
                    }
                    catch( ::std::exception const& e )
                    {
                        natus::log::global_t::error( "[cid] : " +
                            natus::std::string( e.what() ) ) ;
                    }
                }*/                

                void_t aquire( void_t ) noexcept
                {
                    ++_ref_count ;
                }

                bool_t release( void_t ) noexcept
                {
                    --_ref_count ;
                    return _ref_count == 0 ;
                }
            };
            natus_typedef( shared_data ) ;

        private: // variables

            shared_data_ptr_t _sd = nullptr ;

        protected:

            id( size_t const ) : id()
            {
                _sd->aquire() ;
            }

        public:

            id( void_t ) 
            {
                _sd = natus::memory::global_t::alloc< shared_data_t >() ;
            }

            

            id( this_cref_t rhv ) 
            {
                _sd = rhv._sd ;
                if( natus::core::is_not_nullptr(_sd) ) 
                    _sd->aquire() ;
            }

            id( this_rref_t rhv ) 
            {
                natus_move_member_ptr( _sd, rhv ) ;
            }

            virtual ~id( void_t ) 
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
        };
        natus_typedef( id ) ;

        //**********************************************
        template< class T >
        class cid : public id
        {
            natus_this_typedefs( cid<T> ) ;
            natus_typedefs( T, value ) ;

        private:

            

        private:

            value_ptr_t _pptr = nullptr ;

            cid( value_ptr_t ptr ) : id(size_t())
            {                
                _pptr = ptr ;
            }

        public:

            cid( void_t ) noexcept
            {}

            cid( this_cref_t rhv ) : id( rhv ) noexcept
            {}

            cid( this_rref_t rhv ) : id( ::std::move(rhv) ) noexcept
            {}

            cid( value_cref_t rhv ) noexcept
            {
                *this = rhv ;
            }

            cid( value_rref_t rhv ) noexcept
            {
                *this = ::std::move( rhv ) ;
            }
            
            ~cid( void_t ) noexcept
            {}
            
            this_ref_t operator = ( this_cref_t rhv ) noexcept
            {
                static_cast< this_ref_t > ( reinterpret_cast<id_ptr_t>(this)->operator = ( rhv ) ) ;
                
                _pptr = rhv._pptr ;

                return *this ;
            }

            this_ref_t operator = ( this_rref_t rhv ) noexcept
            {
                static_cast<this_ref_t>( reinterpret_cast<id_ptr_t>(this)->operator = ( ::std::move( rhv ) ) ) ;

                natus_move_member_ptr( _pptr, rhv ) ;

                return *this ;
            }

            this_ref_t operator = ( value_cref_t rhv ) noexcept
            {
                return *this = this_t::construct( rhv ) ;
            }

            
            this_ref_t operator = ( value_rref_t rhv ) noexcept
            {
                return *this = this_t::construct( ::std::move( rhv ) ) ;
            }

            
            static this_t construct( T const & t )
            {
                T * ptr = natus::memory::global_t::alloc<T>( T(t),
                    "[shared_data]" ) ;
                                
                return this_t( ptr ) ;
            }

            static this_t construct( T && t )
            {
                T * ptr = natus::memory::global_t::alloc<T>( T( ::std::move(t) ),
                    "[shared_data]" ) ;
                                               
                return this_t( ptr ) ;
            }
        };
    }
}