//------------------------------------------------------------
// snakeoil (c) Alexis Constantin Link
// Distributed under the MIT license
//------------------------------------------------------------
#ifndef _NATUS_CONCURRENT_CONTAINER_DOUBLE_BUFFERED_MAP_HPP_
#define _NATUS_CONCURRENT_CONTAINER_DOUBLE_BUFFERED_MAP_HPP_

#include "../../typedefs.h"
#include <natus/std/container/map.hpp>

namespace natus::concurrent
{
    namespace natus_double_buffered
    {
        template< typename K, typename I >
        class map
        {
            typedef map< K, I > __this_t ;
            natus_this_typedefs( __this_t ) ;

            natus_typedefs( K, key ) ;
            natus_typedefs( I, item ) ;

            typedef natus::std::map< key_t, item_t > __map_t ;
            natus_typedefs( __map_t, map ) ;

        private:

            map_t _maps[2] ;
            size_t _read_index ;

        private:

            size_t __read_index( void_t ) const { return _read_index ; }
            size_t __write_index( void_t ) const { return (_read_index+1) & 1 ; }
            void_t __swap_index( void_t ) { _read_index = __write_index() ; }

        private:

            map_ref_t __read_ref( void_t ) { return _maps[__read_index()] ; }
            map_cref_t __read_ref( void_t ) const { return _maps[__read_index()] ; }

            map_ref_t __write_ref( void_t ) { return _maps[__write_index()] ; }
            map_cref_t __write_ref( void_t ) const { return _maps[__write_index()] ; }

        public:

            map( void_t )
            {
                _read_index = 0 ;
            }

            map( this_rref_t rhv )
            {
                _maps[0] = ::std::move( rhv._maps[0] ) ;
                _maps[1] = ::std::move( rhv._maps[1] ) ;
                _read_index = rhv._read_index ;
            }

        public: // to: write buffer

            /// to: write buffer
            /// places the item at key into the write buffer.
            this_ref_t put( key_cref_t key, item_cref_t item )
            {
                __write_ref()[key] = item ;
                return *this ;
            }

            /// to: write buffer
            /// removes the data at the given key.
            bool_t remove( key_cref_t key )
            {
                return __write_ref().erase( key ) != 0 ;
            }

        public: // to: read buffer

            /// to: read buffer
            /// checks if key is in read buffer.
            bool_t contains( key_cref_t key ) const 
            {
                return __read_ref().find(key) != __read_ref().end() ;
            }

            /// to: read buffer
            /// returns the item at key.
            /// @note
            /// the key must be found.
            bool_t at( key_cref_t key, item_ref_t item_out ) const 
            {
                const auto found_i = __read_ref().find( key ) ;
                const bool_t found = found_i != __read_ref().end();
                if( found ) item_out = found_i->second;
                return found;
            }

            size_t size( void_t ) const 
            {
                return __read_ref().size() ;
            }

        public:

            /// 1. swap the read and write buffers.
            /// 2. clear the write buffer 
            void_t swap_and_clear( void_t ) 
            {
                __swap_index() ;
                __write_ref().clear() ;
            }

            void_t swap_and_overwrite( void_t ) 
            {
                swap_and_clear() ;
                __write_ref() = __read_ref() ;
            }
        };
    }
}

#endif

