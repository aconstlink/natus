#pragma once

#include "../api.h"
#include "../typedefs.h"
#include "../result.h"

namespace natus
{
    namespace gpu
    {
        // strictly unique id. ids flow through
        // the rendering engine. While an id is 
        // traveling through the rendering engine,
        // no other operation can be performed on it.
        class id
        {
            natus_this_typedefs( id ) ;

            // backend id
            size_t _bid = size_t( -1 ) ;

            // object id
            size_t _oid = size_t( -1 ) ;

        public:

            id( void_t ) {}

            id( size_t const bid, size_t const oid )
            {
                _bid = bid ;
                _oid = oid ;
            }
            
            id( this_cref_t ) = delete ;

            id( this_rref_t rhv ) noexcept
            { 
                _bid = rhv._bid ;
                _oid = rhv._oid ;

                rhv._bid = size_t( -1 ) ;
                rhv._oid = size_t( -1 ) ;
            }

            virtual ~id( void_t ) 
            {
                // stuff must be cleaned up!
                natus_assert( _bid == size_t( -1 ) ) ;
                natus_assert( _oid == size_t( -1 ) ) ;
            }

            bool_t is_valid( void_t ) const 
            {
                return _bid != size_t( -1 ) && _oid != size_t( -1 ) ;
            }

            bool_t is_not_valid( void_t ) const 
            {
                return natus::core::is_not( this_t::is_valid() ) ;
            }

            size_t get_oid( void_t ) const noexcept { return _oid ; }
            size_t get_bid( void_t ) const noexcept { return _bid ; }
            bool_t is_bid( size_t const bid ) const { return _bid == bid ; }
            bool_t is_not_bid( size_t const bid ) const { 
                return natus::core::is_not( this_t::is_bid( bid ) ) ; }

        public:

            this_ref_t operator = ( this_rref_t rhv ) noexcept
            {
                _bid = rhv._bid ;
                _oid = rhv._oid ;

                rhv._bid = size_t( -1 ) ;
                rhv._oid = size_t( -1 ) ;

                return *this ;
            }

            this_ref_t operator = ( this_cref_t ) = delete ;

        };
        natus_typedef( id ) ;
    }
}