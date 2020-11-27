#pragma once

#include "typedefs.h"

#include <natus/memory/global.h>
#include <natus/memory/macros.h>

#include <natus/ntd/vector.hpp>

namespace natus
{
    namespace font
    {
        class font_file
        {
            natus_this_typedefs( font_file ) ;

        private:

            uchar_ptr_t _ptr = nullptr ;
            size_t _sib = 0 ;
            natus::ntd::string_t _name ;

        public:

            font_file( void_t ) noexcept {}

            font_file( natus::ntd::string_cref_t name, uchar_cptr_t from, size_t const sib ) noexcept
            {
                _name = name ;
                _ptr = natus::memory::global_t::alloc_raw< uchar_t >( sib ) ;
                _sib = sib ;

                std::memcpy( void_ptr_t(_ptr), void_cptr_t(from), sib ) ;
            }

            font_file( this_cref_t rhv ) noexcept
            {
                _sib = rhv._sib ;
                _ptr = natus::memory::global_t::alloc_raw< uchar_t >( _sib ) ;
                std::memcpy( _ptr, rhv._ptr, rhv._sib ) ;
                _name = rhv._name ;
            }

            font_file( this_rref_t rhv ) noexcept
            {
                _sib = rhv._sib ;
                rhv._sib = 0 ;
                natus_move_member_ptr( _ptr, rhv ) ;
                _name = std::move( rhv._name ) ;
            }

            ~font_file( void_t ) noexcept
            {
                natus::memory::global_t::dealloc_raw( _ptr ) ;
            }

            natus::ntd::string_cref_t name( void_t ) const noexcept{ return _name ; }
            void_cptr_t data( void_t ) const noexcept { return _ptr ; }
            size_t sib( void_t ) const noexcept { return _sib ; }
        };
        natus_typedef( font_file ) ;
        natus_typedefs( natus::ntd::vector< font_file_t >, font_files ) ;
    }
}