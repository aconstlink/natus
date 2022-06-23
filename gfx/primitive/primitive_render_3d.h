#pragma once

#include "../api.h"
#include "../typedefs.h"

#include "line_render_3d.h"
#include "tri_render_3d.h"

namespace natus
{
    namespace gfx
    {
        class NATUS_GFX_API primitive_render_3d
        {
            natus_this_typedefs( primitive_render_3d ) ;

        private: // other render

            natus::ntd::string_t _name ;
            natus::graphics::async_views_t _asyncs ;

            natus::gfx::line_render_3d_res_t _lr ;
            natus::gfx::tri_render_3d_res_t _tr ;

        public:

            primitive_render_3d( void_t ) ;
            primitive_render_3d( this_cref_t ) = delete ;
            primitive_render_3d( this_rref_t ) ;
            ~primitive_render_3d( void_t ) ;

            void_t init( natus::ntd::string_cref_t, natus::graphics::async_views_t ) noexcept ;
            void_t release( void_t ) noexcept ;

        public:

            void_t draw_line( natus::math::vec3f_cref_t p0, natus::math::vec3f_cref_t p1, natus::math::vec4f_cref_t color ) noexcept ;

            void_t draw_tri( natus::math::vec3f_cref_t p0, natus::math::vec3f_cref_t p1, natus::math::vec3f_cref_t p2, natus::math::vec4f_cref_t color ) noexcept ;

            #if 0
            void_t draw_rect( natus::math::vec3f_cref_t p0, natus::math::vec3f_cref_t p1, natus::math::vec3f_cref_t p2, natus::math::vec3f_cref_t p3, natus::math::vec4f_cref_t color, 
                natus::math::vec4f_cref_t border_color ) noexcept ;
            #endif
            void_t draw_circle( natus::math::mat3f_cref_t, natus::math::vec3f_cref_t p0, float_t const r, natus::math::vec4f_cref_t color, 
                natus::math::vec4f_cref_t border_color, size_t const ) noexcept ;

            void_t set_view_proj( natus::math::mat4f_cref_t view, natus::math::mat4f_cref_t proj ) noexcept ;

        public:

            // copy all data to the gpu buffer and transmit the data
            void_t prepare_for_rendering( void_t ) noexcept ;
            void_t render( void_t ) noexcept ;
        };
        natus_res_typedef( primitive_render_3d ) ;
    }
}