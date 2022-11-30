
#pragma once

#include "../object.hpp"

#include "../backend/types.h"
#include "../buffer/vertex_buffer.hpp"
#include "../buffer/index_buffer.hpp"
#include <natus/ntd/vector.hpp>

namespace natus
{
    namespace graphics
    {
        class NATUS_GRAPHICS_API geometry_object : public object
        {
            natus_this_typedefs( geometry_object ) ;

        private:

            natus::graphics::primitive_type _pt ;
            natus::graphics::vertex_buffer_t _vb ;
            natus::graphics::index_buffer_t _ib ;

            natus::ntd::string_t _name ;

        public: 

            geometry_object( void_t ) noexcept {}

            geometry_object( natus::ntd::string_cref_t name ) noexcept : _name(name)
            {}

            geometry_object( natus::ntd::string_cref_t name, natus::graphics::primitive_type const pt, 
                natus::graphics::vertex_buffer_cref_t vb, natus::graphics::index_buffer_cref_t ib ) noexcept
            {
                _name = name ;
                _pt = pt ;
                _vb = vb ;
                _ib = ib ;
            }

            geometry_object( natus::ntd::string_cref_t name, natus::graphics::primitive_type const pt,
                natus::graphics::vertex_buffer_rref_t vb, natus::graphics::index_buffer_rref_t ib ) noexcept
            {
                _name = name ;
                _pt = pt ;
                _vb = ::std::move( vb ) ;
                _ib = ::std::move( ib ) ;
            }

            geometry_object( natus::ntd::string_cref_t name, natus::graphics::primitive_type const pt, 
                natus::graphics::vertex_buffer_cref_t vb ) noexcept
            {
                _name = name ;
                _pt = pt ;
                _vb = vb ;
            }

            geometry_object( natus::ntd::string_cref_t name, natus::graphics::primitive_type const pt,
                natus::graphics::vertex_buffer_rref_t vb  ) noexcept
            {
                _name = name ;
                _pt = pt ;
                _vb = std::move( vb ) ;
            }

            geometry_object( this_cref_t rhv ) noexcept : object( rhv ) 
            {
                _pt = rhv._pt ;
                _vb = rhv._vb ;
                _ib = rhv._ib ;
                _name = rhv._name ;
            }

            geometry_object( this_rref_t rhv ) noexcept : object( ::std::move( rhv ) )
            {
                _pt = rhv._pt ;
                _vb = ::std::move( rhv._vb ) ;
                _ib = ::std::move( rhv._ib ) ;
                _name = ::std::move( rhv._name ) ;
            }

            ~geometry_object( void_t ) noexcept
            {}

            this_ref_t operator = ( this_cref_t rhv ) noexcept
            {
                object::operator=( rhv ) ;

                _pt = rhv._pt ;
                _vb = rhv._vb ;
                _ib = rhv._ib ;
                _name = rhv._name ;
                return *this ;
            }

            this_ref_t operator = ( this_rref_t rhv ) noexcept
            {
                object::operator=( ::std::move( rhv ) ) ;

                _pt = rhv._pt ;
                _vb = ::std::move( rhv._vb ) ;
                _ib = ::std::move( rhv._ib ) ;
                _name = ::std::move( rhv._name ) ;
                return *this ;
            }

        public:

            natus::graphics::primitive_type primitive_type( void_t ) const noexcept 
            {
                return _pt ;
            }

            natus::graphics::vertex_buffer_ref_t vertex_buffer( void_t ) noexcept
            {
                return _vb ;
            }

            natus::graphics::vertex_buffer_cref_t vertex_buffer( void_t ) const noexcept
            {
                return _vb ;
            }

            natus::graphics::index_buffer_ref_t index_buffer( void_t ) noexcept 
            {
                return _ib ;
            }

            natus::graphics::index_buffer_cref_t index_buffer( void_t ) const noexcept
            {
                return _ib ;
            }

            natus::ntd::string_cref_t name( void_t ) const noexcept
            {
                return _name ;
            }
        };
        natus_res_typedef( geometry_object ) ;
    }
}
