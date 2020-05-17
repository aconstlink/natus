
#pragma once

#include "../types.h"
#include "vertex_attribute.h"

#include <natus/std/vector.hpp>

#include <cstring>
#include <algorithm>

namespace natus
{
    namespace gpu
    {
        struct vertex_buffer
        {
            natus_this_typedefs( vertex_buffer ) ;

        public:

            struct data
            {
                natus::gpu::vertex_attribute va ;
                natus::gpu::type type ;
                natus::gpu::type_struct type_struct ;
            };
            natus_typedef( data ) ;
            natus_typedefs( natus::std::vector< data_t >, datas ) ;

        private:

            datas_t _vertex_layout ;

            size_t _num_elems = 0 ;
            void_ptr_t _data = nullptr ;

            natus::gpu::usage_type _ut = usage_type::buffer_static ;

        public:

            vertex_buffer( void_t ) {}
            vertex_buffer( natus::gpu::usage_type const ut ) : _ut(ut){}
            vertex_buffer( this_cref_t rhv ) 
            {
                *this = rhv ;
            }

            vertex_buffer( this_rref_t rhv )
            {
                *this = ::std::move( rhv ) ;
            }

            ~vertex_buffer( void_t )
            {
                natus::memory::global_t::dealloc( _data ) ;
            }

            this_ref_t operator = ( this_cref_t rhv ) noexcept 
            {
                _vertex_layout = rhv._vertex_layout ;
                this_t::resize( rhv._num_elems ) ;
                ::std::memcpy( _data, rhv._data, rhv.get_sib() ) ;
                return *this ;
            }

            this_ref_t operator = ( this_rref_t rhv ) noexcept
            {
                _vertex_layout = ::std::move( rhv._vertex_layout ) ;
                _num_elems = rhv._num_elems ;
                natus_move_member_ptr( _data, rhv ) ;
                return *this ;
            }

        public:

            this_ref_t resize( size_t const ne )
            {
                _num_elems = ne ;
                size_t const sib = this_t::get_sib() ;
                natus::memory::global_t::dealloc( _data ) ;
                _data = natus::memory::global_t::alloc( sib, "vertex buffer" ) ;

                return *this ;
            }

            
            template< typename vertex_t >
            this_ref_t update( ::std::function< void_t ( vertex_t * array, size_t const ne ) > funk )
            {
                funk( static_cast< vertex_t* >( _data ), _num_elems ) ;
                return *this ;
            }

            void_cptr_t data( void_t ) const noexcept { return _data ; }

            this_ref_t add_layout_element( natus::gpu::vertex_attribute const va, 
                natus::gpu::type const t, natus::gpu::type_struct const ts )
            {
                this_t::data_t d ;
                d.va = va ;
                d.type = t ;
                d.type_struct = ts ;

                _vertex_layout.push_back( d ) ;

                return *this ;
            }
            
            void_t for_each_layout_element( ::std::function< void_t ( data_cref_t d ) > funk )
            {
                for( auto & le : _vertex_layout )
                {
                    funk( le ) ;
                }
            }

            size_t get_layout_element_sib( natus::gpu::vertex_attribute const va )
            {
                auto iter = ::std::find_if( _vertex_layout.begin(), _vertex_layout.end(), 
                    [&]( data_cref_t d )
                {
                    return d.va == va ;
                } ) ;

                if( iter == _vertex_layout.end() ) return 0 ;

                size_t const sib = natus::gpu::size_of( iter->type ) *
                    natus::gpu::size_of( iter->type_struct ) ;

                return sib ;
            }

            size_t get_layout_sib( void_t ) const noexcept
            {
                size_t sib = 0 ;
                for( auto const& d : _vertex_layout )
                {
                    sib += natus::gpu::size_of( d.type ) *
                        natus::gpu::size_of( d.type_struct ) ;
                }
                return sib ;
            }

            size_t get_sib( void_t ) const noexcept 
            {
                return this_t::get_layout_sib() * _num_elems ;
            }

            void_t copy_done( void_t )
            {
                if( _ut == usage_type::buffer_static )
                    this_t::resize( 0 ) ;
            }

            size_t get_num_elements( void_t ) const noexcept { return _num_elems ; }
        };
        natus_typedef( vertex_buffer ) ;
    }
}
