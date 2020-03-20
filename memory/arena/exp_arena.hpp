#pragma once

#include "../typedefs.h"
#include "../global.h"

#include <natus/core/macros/move.h>

#include <mutex>
#include <vector>

namespace natus
{
    namespace memory
    {
        namespace experimental
        {
            template< typename T >
            class arena
            {
                natus_this_typedefs( arena<T> ) ;
                natus_typedefs( T, type ) ;

                struct page
                {
                    natus_this_typedefs( page ) ;

                public:

                    page( void_t ) {}
                    page( this_cref_t ) = delete ;
                    page( this_rref_t rhv ) {
                        natus_move_member_ptr( mem_ptr, rhv ) ;
                        free_elems = rhv.free_elems ;
                        rhv.free_elems = 0 ;

                        natus_move_member_ptr( free_ptr, rhv ) ;
                        free_pos = rhv.free_pos ;
                        rhv.free_pos = 0 ;
                    }

                    ~page( void_t )
                    {}

                public:

                    ::std::mutex mtx ;
                    size_t free_elems = 0 ;
                    byte_ptr_t mem_ptr = nullptr ;
                
                    size_t free_pos = 0 ;
                    size_t * free_ptr = nullptr ;

                };
                natus_typedef( page ) ;
                natus_typedefs( ::std::vector< page_ptr_t >, pages ) ;

            private:

                size_t _prealloc = 100 ;

                ::std::mutex _mtx ;
                pages_t _pages ;

            public:

                arena( void_t )
                {
                    this_t::alloc_page( _prealloc ) ;
                }

                arena( size_t const prealloc )
                {
                    _prealloc = prealloc ;
                    this_t::alloc_page( _prealloc ) ;
                }

                arena( this_cref_t ) = delete ;
                arena( this_rref_t rhv )
                {
                    _prealloc = rhv._prealloc ;
                    _pages = ::std::move( rhv._pages ) ;
                }

                ~arena( void_t ) 
                {
                    for( auto * p : _pages )
                    {
                        natus::memory::global::dealloc( p->mem_ptr ) ;
                        natus::memory::global::dealloc( p->free_ptr ) ;
                        natus::memory::global::dealloc( p ) ;
                    }
                }

            public:

                //**********************************************************************************
                type_ptr_t alloc( type_rref_t rhv )
                {
                    page_ptr_t use_ptr = nullptr ;

                    // 1. find a page with free memory
                    {
                        ::std::lock_guard< ::std::mutex > lk( _mtx ) ;
                        for( auto * p : _pages )
                        {
                            if( p->free_elems > 0 )
                            {
                                use_ptr = p ;
                                break ;
                            }
                        }
                    }

                    // 2. if no page is found, allocate a new page
                    if( natus::core::is_nullptr( use_ptr ) )
                    {
                        use_ptr = this_t::alloc_page( _prealloc ) ;
                    }

                    size_t index = 0 ;

                    // 3. get a free location
                    {
                        ::std::lock_guard< ::std::mutex > lk( use_ptr->mtx ) ;
                        size_t const free_pos = use_ptr->free_pos++ ;
                        use_ptr->free_elems-- ;

                        index = use_ptr->free_ptr[ free_pos ] ;
                    }

                    size_t const access_loc = index *
                        ( sizeof( T ) + sizeof( size_t ) + sizeof( page_ptr_t ) ) ;
    
                    // 4. shift pointer for info storage
                    byte_ptr_t ptr_start = use_ptr->mem_ptr + access_loc ;

                    byte_ptr_t ptr_return = ptr_start + 
                        sizeof( size_t ) + sizeof( page_ptr_t ) ;

                    // 5. store page pointer
                    {
                        page_ptr_t * tmp_ptr = ( page_ptr_t * ) ptr_start ;
                        *tmp_ptr = use_ptr ;
                    }

                    // 6. store free pos
                    {
                        size_t * tmp_ptr = ( size_t * ) (ptr_start+sizeof(page_ptr_t)) ;
                        *tmp_ptr = index ;
                    }


                    return new( ptr_return ) T( ::std::move( rhv ) ) ;
                }

                //**********************************************************************************
                type_ptr_t alloc( void_t )
                {
                    return this_t::alloc( type_t() ) ;
                }

                //**********************************************************************************
                void_t dealloc( type_ptr_t ptr )
                {
                    byte_ptr_t ptr_start = byte_ptr_t(ptr) - sizeof( size_t ) - sizeof(page_ptr_t) ;

                    page_ptr_t page_ptr = *(( page_ptr_t * ) ptr_start) ;
                    size_t const index = *( ( size_t* ) ( ptr_start + sizeof( page_ptr_t ) ) ) ;

                    ptr->~type_t() ;

                    // return memory
                    {
                        ::std::lock_guard< ::std::mutex > lk( page_ptr->mtx ) ;
                        page_ptr->free_elems++ ;
                        page_ptr->free_pos = --page_ptr->free_pos ;
                        page_ptr->free_ptr[ page_ptr->free_pos ] = index ;
                    }
                }

            private:

                //**********************************************************************************
                page_ptr_t alloc_page( size_t const prealloc )
                {
                    page_t p ;

                    // 1. the memory block itself
                    {
                        size_t const sib = prealloc * sizeof( T ) ;
                        size_t const sibx = prealloc * ( sizeof( size_t ) << 1 ) ;

                        byte_ptr_t raw_mem = natus::memory::global::alloc_raw<byte_t>( sib + sibx,
                            "[natus::memory::arena] : raw memory data" ) ;

                        p.mem_ptr = raw_mem ;
                    }

                    // 2. the freelist
                    {
                        size_t const sib = prealloc * sizeof( size_t ) ;
                        byte_ptr_t raw_mem = natus::memory::global::alloc_raw<byte_t>( sib,
                            "[natus::memory::arena] : raw memory freelist" ) ;

                        p.free_ptr = (size_t*)raw_mem ;

                        {
                            size_t * ptr = ( size_t* ) raw_mem ;
                            for( size_t i = 0; i < prealloc; ++i )
                            {
                                ptr[ i ] = i ;
                            }
                        }
                    }

                    // 3. properties
                    {
                        p.free_elems = prealloc ;
                    }

                    page_ptr_t ret = natus::memory::global::alloc( ::std::move( p ),
                        "[arena::alloc_page] : page" ) ;

                    {
                        ::std::lock_guard< ::std::mutex > lk( _mtx ) ;
                        _pages.push_back( ret ) ;
                    }

                    return ret ;
                }
            };
        }
    }
   
}