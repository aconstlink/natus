
#pragma once

#include "../object.hpp"

#include "../backend/types.h"
#include "../buffer/vertex_buffer.hpp"
#include "../buffer/index_buffer.hpp"
#include <natus/std/vector.hpp>

namespace natus
{
    namespace gpu
    {
        class NATUS_GPU_API geometry_configuration : public object
        {
            natus_this_typedefs( geometry_configuration ) ;

        private:

            natus::gpu::primitive_type _pt ;
            natus::gpu::vertex_buffer_t _vb ;
            natus::gpu::index_buffer_t _ib ;

            natus::std::string_t _name ;

        public: 

            geometry_configuration( void_t ) {}

            geometry_configuration( natus::std::string_cref_t name ) : _name(name)
            {}

            geometry_configuration( natus::std::string_cref_t name, natus::gpu::primitive_type const pt, 
                natus::gpu::vertex_buffer_cref_t vb, natus::gpu::index_buffer_cref_t ib )
            {
                _name = name ;
                _pt = pt ;
                _vb = vb ;
                _ib = ib ;
            }

            geometry_configuration( natus::std::string_cref_t name, natus::gpu::primitive_type const pt,
                natus::gpu::vertex_buffer_rref_t vb, natus::gpu::index_buffer_rref_t ib )
            {
                _name = name ;
                _pt = pt ;
                _vb = ::std::move( vb ) ;
                _ib = ::std::move( ib ) ;
            }

            geometry_configuration( this_cref_t rhv ) : object( rhv ) 
            {
                _pt = rhv._pt ;
                _vb = rhv._vb ;
                _ib = rhv._ib ;
                _name = rhv._name ;
            }

            geometry_configuration( this_rref_t rhv ) : object( ::std::move( rhv ) )
            {
                _pt = rhv._pt ;
                _vb = ::std::move( rhv._vb ) ;
                _ib = ::std::move( rhv._ib ) ;
                _name = ::std::move( rhv._name ) ;
            }

            ~geometry_configuration( void_t ) 
            {}

            this_ref_t operator = ( this_cref_t rhv )
            {
                object::operator=( rhv ) ;

                _pt = rhv._pt ;
                _vb = rhv._vb ;
                _ib = rhv._ib ;
                _name = rhv._name ;
                return *this ;
            }

            this_ref_t operator = ( this_rref_t rhv )
            {
                object::operator=( ::std::move( rhv ) ) ;

                _pt = rhv._pt ;
                _vb = ::std::move( rhv._vb ) ;
                _ib = ::std::move( rhv._ib ) ;
                _name = ::std::move( rhv._name ) ;
                return *this ;
            }

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

            natus::std::string_cref_t name( void_t ) const noexcept
            {
                return _name ;
            }
        };
        natus_soil_typedef( geometry_configuration ) ;
    }
}
