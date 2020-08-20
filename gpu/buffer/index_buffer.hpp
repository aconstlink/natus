
#pragma once

#include "../types.h"
#include "vertex_attribute.h"

#include <natus/ntd/vector.hpp>
#include <cstring>

namespace natus
{
    namespace gpu
    {
        struct index_buffer
        {
            natus_this_typedefs( index_buffer ) ;

        private:

            natus::gpu::type _type ;
            size_t _num_elems = 0 ;
            void_ptr_t _data = nullptr ;

        public:

            index_buffer( void_t ) {}
            index_buffer( this_cref_t rhv )
            {
                *this = rhv ;
            }

            index_buffer( this_rref_t rhv )
            {
                *this = ::std::move( rhv ) ;
            }

            ~index_buffer( void_t ) 
            {
                natus::memory::global_t::dealloc( _data ) ;
            }

            this_ref_t operator = ( this_cref_t rhv ) noexcept
            {
                _type = rhv._type ;
                this_t::resize( rhv._num_elems ) ;
                ::std::memcpy( _data, rhv._data, rhv.get_sib() ) ;
                return *this ;
            }

            this_ref_t operator = ( this_rref_t rhv ) noexcept
            {
                _type = rhv._type ;
                _num_elems = rhv._num_elems ;
                natus_move_member_ptr( _data, rhv ) ;
                return *this ;
            }

        public:

            this_ref_t resize( size_t const ne )
            {
                _num_elems = ne ;
                size_t const sib = this_t::get_sib() ;
                _data = natus::memory::global_t::alloc( sib, "index buffer" ) ;
                return *this ;
            }

            void_cptr_t data( void_t ) const noexcept { return _data ; }

            template< typename index_t >
            this_ref_t update( ::std::function< void_t ( index_t* array, size_t const ne ) > funk )
            {
                funk( static_cast< index_t* >( _data ), _num_elems ) ;
                return *this ;
            }

            this_ref_t set_layout_element( natus::gpu::type const t ) noexcept
            {
                _type = t ;
                return *this ;
            }

            size_t get_element_sib( void_t ) const noexcept
            {
                return natus::gpu::size_of( _type ) ;
            }

            size_t get_sib( void_t ) const noexcept
            {
                return this_t::get_element_sib() * _num_elems ;
            }

            size_t get_num_elements( void_t ) const noexcept { return _num_elems ; }
        };
        natus_typedef( index_buffer ) ;
    }
}
