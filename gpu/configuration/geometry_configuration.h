
#pragma once

#include "../api.h"
#include "../protos.h"
#include "../typedefs.h"

#include "../backend/types.h"
#include "../buffer/vertex_buffer.hpp"
#include "../buffer/index_buffer.hpp"
#include <natus/std/vector.hpp>

namespace natus
{
    namespace gpu
    {
        class NATUS_GPU_API geometry_configuration
        {
            natus_this_typedefs( geometry_configuration ) ;

        private:

            natus::gpu::primitive_type _pt ;
            natus::gpu::vertex_buffer_t _vb ;
            natus::gpu::index_buffer_t _ib ;

        public: 

            geometry_configuration( void_t ) 
            {}

            geometry_configuration( natus::gpu::primitive_type const pt, 
                natus::gpu::vertex_buffer_cref_t vb, natus::gpu::index_buffer_cref_t ib )
            {
                _pt = pt ;
                _vb = vb ;
                _ib = ib ;
            }

            geometry_configuration( this_cref_t rhv ) 
            {
                _pt = rhv._pt ;
                _vb = rhv._vb ;
                _ib = rhv._ib ;
            }

            geometry_configuration( this_rref_t rhv )
            {
                _pt = rhv._pt ;
                _vb = ::std::move( rhv._vb ) ;
                _ib = ::std::move( rhv._ib ) ;
            }

            ~geometry_configuration( void_t ) 
            {}

        public:

            natus::gpu::primitive_type primitive_type( void_t ) const noexcept 
            {
                return _pt ;
            }

            natus::gpu::vertex_buffer_ref_t vertex_buffer( void_t ) noexcept
            {
                return _vb ;
            }

            natus::gpu::vertex_buffer_cref_t vertex_buffer( void_t ) const noexcept
            {
                return _vb ;
            }

            natus::gpu::index_buffer_ref_t index_buffer( void_t ) noexcept 
            {
                return _ib ;
            }

            natus::gpu::index_buffer_cref_t index_buffer( void_t ) const noexcept
            {
                return _ib ;
            }
        };
        natus_soil_typedef( geometry_configuration ) ;
    }
}