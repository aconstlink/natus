
#pragma once

#include "../icontext.h"

#include <natus/ntd/string.hpp>
#include <natus/math/vector/vector4.hpp>

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

            virtual void_t activate_framebuffer( void_t ) noexcept = 0 ;
            virtual void_t clear_render_target_view( natus::math::vec4f_cref_t ) noexcept = 0 ;
            virtual void_t clear_depth_stencil_view( void_t ) noexcept = 0 ;
        };
        natus_res_typedef( d3d11_context ) ;
    }
}