#pragma once

#include "../../typedefs.h"

namespace natus
{
    namespace concurrent
    {
        namespace natus_double_buffered
        {
            /// allows to double buffer an item.
            /// @note
            /// 1. only one write item
            /// 2. only one read item
            template< typename T >
            class item
            {
                natus_this_typedefs( item<T> ) ;
                natus_typedefs( T, item ) ;

            public:

                typedef ::std::function< void ( item_ref_t ) > change_funk_t ;

            private:

                item_t _items[ 2 ] ;
                size_t _read_index ;

            private:

                size_t __read_index( void_t ) const { return _read_index ; }
                size_t __write_index( void_t ) const { return ( _read_index + 1 ) & 1 ; }
                void_t __swap_index( void_t ) { _read_index = __write_index() ; }

                item_ref_t __read_ref( void_t ) { return _items[ __read_index() ] ; }
                item_cref_t __read_ref( void_t ) const { return _items[ __read_index() ] ; }

                item_ref_t __write_ref( void_t ) { return _items[ __write_index() ] ; }
                item_cref_t __write_ref( void_t ) const { return _items[ __write_index() ] ; }

            public:

                item( void_t )
                {
                    _read_index = 0 ;
                }

                item( this_rref_t rhv )
                {
                    _read_index = rhv._read_index ;
                    _items[ 0 ] = rhv._items[ 0 ] ;
                    _items[ 1 ] = rhv._items[ 1 ] ;
                }

                item( this_cref_t rhv )
                {
                    _read_index = rhv._read_index ;
                    _items[ 0 ] = rhv._items[ 0 ] ;
                    _items[ 1 ] = rhv._items[ 1 ] ;
                }

            public:

                /// to: write buffer
                /// sets the write item.
                void_t set( item_cref_t the_item )
                {
                    __write_ref() = the_item ;
                }

                /// to: write buffer
                /// allows to change the write item by a function.
                /// especially useful for complex items.
                void_t set( change_funk_t funk )
                {
                    funk( __write_ref() ) ;
                }

                /// to: read buffer
                /// gets the read item.
                item_cref_t get( void_t ) const
                {
                    return __read_ref() ;
                }

            public:

                /// just swaps the read/write indices.
                void_t swap( void_t )
                {
                    __swap_index() ;
                }
            };
        }
    }
}
