#pragma once

#include "../api.h"
#include "../protos.h"
#include "../typedefs.h"

#include "shader.hpp"

namespace natus
{
    namespace gpu
    {
        class NATUS_GPU_API shader_set
        {
            natus_this_typedefs( shader_set ) ;

        private:

            natus::gpu::shader_t _vs ;
            natus::gpu::shader_t _ps ;
            natus::gpu::shader_t _gs ;

        public:

            shader_set( void_t ) 
            {}

            shader_set( this_cref_t rhv ) noexcept
            {
                *this = rhv ;
            }

            shader_set( this_rref_t rhv ) noexcept
            {
                *this = ::std::move( rhv ) ;
            }

            this_ref_t operator = ( this_cref_t rhv ) noexcept
            {
                _vs = rhv._vs ;
                _ps = rhv._ps ;
                _gs = rhv._gs ;

                return *this ;
            }

            this_ref_t operator = ( this_rref_t rhv ) noexcept
            {
                _vs = ::std::move( rhv._vs ) ;
                _ps = ::std::move( rhv._ps ) ;
                _gs = ::std::move( rhv._gs ) ;

                return *this ;
            }

        public:

            this_ref_t set_vertex_shader( natus::gpu::shader_in_t s ) noexcept
            {
                _vs = s ;
                return *this ;
            }

            this_ref_t set_pixel_shader( natus::gpu::shader_t s ) noexcept
            {
                _ps = s ;
                return *this ;
            }

            this_ref_t set_geometry_shader( natus::gpu::shader_t s ) noexcept
            {
                _gs = s ;
                return *this ;
            }

            natus::gpu::shader_cref_t vertex_shader( void_t ) const noexcept
            {
                return _vs ;
            }

            natus::gpu::shader_cref_t pixel_shader( void_t ) const noexcept
            {
                return _ps ;
            }

            natus::gpu::shader_cref_t geometry_shader( void_t ) const noexcept
            {
                return _gs ;
            }

            bool_t has_vertex_shader( void_t ) const noexcept
            {
                return natus::core::is_not( _vs.code().empty() ) ;
            }

            bool_t has_geometry_shader( void_t ) const noexcept
            {
                return natus::core::is_not( _gs.code().empty() ) ;
            }

            bool_t has_pixel_shader( void_t ) const noexcept
            {
                return natus::core::is_not( _ps.code().empty() ) ;
            }
        };
        natus_typedef( shader_set ) ;
    }
}