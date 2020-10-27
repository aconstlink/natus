
#pragma once

#include "../icontext.h"

#include <natus/ntd/string.hpp>

#include <d3d11.h>
#include <d3d11_1.h>

namespace natus
{
    namespace graphics
    {
        class NATUS_GRAPHICS_API d3d11_context : public natus::graphics::icontext
        {
        public:

            virtual ~d3d11_context( void_t ) noexcept {}

        public:

            virtual ID3D11DeviceContext * ctx( void_t ) noexcept = 0 ;
            virtual ID3D11Device * dev( void_t ) noexcept = 0 ;
        };
        natus_res_typedef( d3d11_context ) ;
    }
}