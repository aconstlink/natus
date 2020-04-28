
#pragma once

#include "../types.h"
#include "vertex_attribute.h"

#include <natus/std/vector.hpp>

namespace natus
{
    namespace gpu
    {
        struct vertex_buffer
        {
            natus_this_typedefs( vertex_buffer ) ;

        private:

            struct data
            {
                natus::gpu::vertex_attribute va ;
                natus::gpu::type type ;
                natus::gpu::type_struct type_struct ;
            };
            natus_typedef( data ) ;
            natus_typedefs( natus::std::vector< data_t >, datas ) ;
            datas_t _vertex_layout ;

            size_t _num_elems = 0 ;

        public:

            vertex_buffer( void_t ) {}
            vertex_buffer( size_t const num_elems ) : _num_elems(num_elems) {}
            vertex_buffer( this_cref_t rhv ) 
            {
                _vertex_layout = rhv._vertex_layout ;
                _num_elems = rhv._num_elems;
            }

            vertex_buffer( this_rref_t rhv )
            {
                _vertex_layout = ::std::move( rhv._vertex_layout ) ;
                _num_elems = rhv._num_elems ;
            }

            ~vertex_buffer( void_t ){}

            this_ref_t operator = ( this_cref_t rhv ) noexcept 
            {
                _vertex_layout = rhv._vertex_layout ;
                _num_elems = rhv._num_elems;
                return *this ;
            }

            this_ref_t operator = ( this_rref_t rhv ) noexcept
            {
                _vertex_layout = ::std::move( rhv._vertex_layout ) ;
                _num_elems = rhv._num_elems;
                return *this ;
            }

        public:

            void_t set_size( size_t const ne )
            {
                _num_elems = ne ;
            }

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

            size_t get_sib( void_t ) const noexcept 
            {
                size_t sib = 0 ;
                for( auto const & d : _vertex_layout )
                {
                    sib += natus::gpu::size_of( d.type ) + 
                        natus::gpu::size_of( d.type_struct ) ;
                }
                return sib * _num_elems ;
            }
        };
        natus_typedef( vertex_buffer ) ;
    }
}