
#pragma once

#include "../icontext.h"

#include <natus/ntd/string.hpp>

namespace natus
{
    namespace graphics
    {
        class NATUS_GRAPHICS_API d3d11_context : public natus::graphics::icontext
        {
        public:

            virtual ~d3d11_context( void_t ) noexcept {}

        public:

            virtual bool_t dummy( natus::ntd::string_cref_t ) const noexcept = 0 ;

        };
        natus_res_typedef( d3d11_context ) ;
    }
}