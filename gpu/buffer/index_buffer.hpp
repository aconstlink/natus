
#pragma once

#include "../types.h"
#include "vertex_attribute.h"

#include <natus/std/vector.hpp>

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

        public:

            index_buffer( void_t ) {}
            index_buffer( size_t const num_elems ) : _num_elems( num_elems ){}
            index_buffer( this_cref_t rhv )
            {
                _type = rhv._type ;
                _num_elems = rhv._num_elems;
            }

            index_buffer( this_rref_t rhv )
            {
                _type = rhv._type ;
                _num_elems = rhv._num_elems;
            }

            ~index_buffer( void_t ) {}

            this_ref_t operator = ( this_cref_t rhv ) noexcept
            {
                _type = rhv._type ;
                _num_elems = rhv._num_elems;
                return *this ;
            }

            this_ref_t operator = ( this_rref_t rhv ) noexcept
            {
                _type = rhv._type ;
                _num_elems = rhv._num_elems;
                return *this ;
            }

        public:

            void_t set_size( size_t const ne )
            {
                _num_elems = ne ;
            }

            this_ref_t set_layout_element( natus::gpu::type const t ) noexcept
            {
                _type = t ;
                return *this ;
            }

            size_t get_sib( void_t ) const noexcept
            {
                return natus::gpu::size_of( _type ) * _num_elems ;
            }
        };
        natus_typedef( index_buffer ) ;
    }
}