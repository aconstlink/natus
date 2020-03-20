//------------------------------------------------------------
// snakeoil (c) Alexis Constantin Link
// Distributed under the MIT license
//------------------------------------------------------------
#ifndef _NATUS_CONCURRENT_CONTAINER_DOUBLE_BUFFERED_VECTOR_HPP_
#define _NATUS_CONCURRENT_CONTAINER_DOUBLE_BUFFERED_VECTOR_HPP_

#include "../../typedefs.h"

#include <natus/std/container/vector.hpp>

namespace natus::concurrent
{
    namespace natus_double_buffered
    {
        /// non-locked double buffered vector
        /// contains two stl vectors that can be read/written in a
        /// double buffered manor. 
        /// 1. Only the write buffer is written to.
        /// 2. Only the read buffer is read from.
        /// 3. The swap operation exchanges these buffers.
        /// The container need to be locked manually for a user scenario.
        /// @note
        /// note that this vector is NOT locked, so the user needs to lock it herself,
        /// whenever concurrent access is required.
        /// Example scenario: 
        /// 1. the read thread doesn't need to lock the buffer at all.
        /// 2. the write thread locks during insertion.
        /// 3. the read thread swaps the container if required with a locked swap.
        template< typename T >
        class vector
        {
            natus_this_typedefs( vector< T > ) ;
            natus_typedefs( T, type ) ;           
            natus_typedefs( natus::std::vector< type_t >, items ) ;
            
            struct buffer
            {
                buffer( void_t )
                {
                    cur_pos = 0 ;
                }

                items_t items ;
                size_t cur_pos ;
            } ;
            natus_typedef( buffer ) ;

        private:

            buffer_t _buffers[2] ;
            size_t _read_index ;

        private:

            size_t __read_index( void_t ) const { return _read_index ; }
            size_t __write_index( void_t ) const { return (_read_index+1) & 1 ; }
            void_t __swap_index( void_t ) { _read_index = __write_index() ; }

        private:

            buffer_ref_t __read_buffer_ref( void_t ) { return _buffers[__read_index()] ; }
            buffer_cref_t __read_buffer_ref( void_t ) const { return _buffers[__read_index()] ; }

            buffer_ref_t __write_buffer_ref( void_t ) { return _buffers[__write_index()] ; }
            buffer_cref_t __write_buffer_ref( void_t ) const { return _buffers[__write_index()] ; }

            size_t __read_pos( void_t ) const { return __read_buffer_ref().cur_pos ; }
            size_t __write_pos( void_t ) const { return __write_buffer_ref().cur_pos ; }

            items_ref_t __read_items_ref( void_t ) { return __read_buffer_ref().items ; }
            items_cref_t __read_items_ref( void_t ) const { return __read_buffer_ref().items ; }
            items_ref_t __write_items_ref( void_t ) { return __write_buffer_ref().items ; }

        public:

            vector( void_t )
            {
                _read_index = 0 ;
            }

        public:

            /// to: write buffer
            /// push back an item to the write buffer.
            this_ref_t push_back( type_cref_t item )
            {
                if( __write_items_ref().size() == __write_pos() )
                {
                    __write_items_ref().resize( __write_pos() + 50 ) ;
                }

                __write_items_ref()[__write_buffer_ref().cur_pos++] = item ;
                return *this ;
            }
            
            /// to: read buffer
            /// returns the read buffer position.
            size_t size( void_t ) const 
            {
                return __read_pos() ;
            }

            /// to: read buffer
            /// returns the item at i of the read buffer.
            type_cref_t at( size_t i ) const
            {
                natus_assert( i < __read_pos() ) ;
                return __read_items_ref()[i] ;
            }

        public:

            /// 1. swap the read/write buffers.
            /// 2. reset write buffer position after swap.
            void_t swap( void_t ) 
            {
                __swap_index() ;
                __write_buffer_ref().cur_pos = 0 ;
            }

        };
    }
}

#endif

