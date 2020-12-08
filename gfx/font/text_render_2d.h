#pragma once

#include "../protos.h"
#include "../result.h"
#include "../api.h"

#include <natus/font/glyph_atlas.h>

#include <natus/io/location.hpp>
#include <natus/io/database.h>
#include <natus/graphics/async.h>
#include <natus/concurrent/mutex.hpp>

#include <natus/math/vector/vector2.hpp>
#include <natus/math/vector/vector3.hpp>
#include <natus/math/matrix/matrix4.hpp>

#include <natus/ntd/vector.hpp>

#include <functional>

namespace natus
{
    namespace gfx
    {
        class NATUS_GFX_API text_render_2d
        {
            natus_this_typedefs( text_render_2d ) ;

        private:
            
            natus::font::glyph_atlas_res_t _ga ;

            natus::ntd::string_t _name ;

        private: // default matrices

            natus::math::mat4f_t _proj ;
            natus::math::mat4f_t _view ;

        private:

            struct vertex 
            { 
                natus::math::vec2f_t pos ; 
            } ;

            natus::graphics::render_object_res_t _rc ;
            natus::graphics::shader_object_res_t _sc ;
            natus::graphics::geometry_object_res_t _gc ;
            natus::graphics::image_object_res_t _ic ;

            natus::ntd::vector< natus::graphics::variable_set_res_t > _vars ;
            natus::graphics::state_object_res_t  _render_states ;

            /// storing the glyph infos of the glyph atlas
            natus::graphics::array_object_res_t _glyph_infos ;
            /// storing the text infos for rendering
            natus::graphics::array_object_res_t _text_infos ;

            natus::graphics::async_views_t _asyncs ;

            size_t num_quads = 1000 ;

        private:

            struct render_info
            {
                size_t start = 0 ;
                size_t num_elems = 0 ;
            };
            natus_typedef( render_info ) ;

            struct glyph_info
            {
                size_t offset ;
                natus::math::vec2f_t pos ;
                natus::math::vec3f_t color ;

                float_t point_size_scale ;
            };
            natus_typedef( glyph_info ) ;

            struct group_info
            {
                natus_this_typedefs( group_info ) ;

                render_info_t ri ;

                natus::concurrent::mutex_t mtx ;
                natus::ntd::vector< glyph_info > glyph_infos ;
                
                natus::math::mat4f_t proj ;
                natus::math::mat4f_t view ;

                group_info( void_t ){}
                group_info( this_cref_t rhv ){
                    glyph_infos = rhv.glyph_infos ;
                    proj = rhv.proj ;
                    view = rhv.view ;
                }
                group_info( this_rref_t rhv ) {
                    glyph_infos = std::move( rhv.glyph_infos ) ;
                    proj = rhv.proj ;
                    view = rhv.view ;
                }
                ~group_info( void_t ) {}

                this_ref_t operator = ( this_cref_t rhv ) {
                    glyph_infos = rhv.glyph_infos ;
                    proj = rhv.proj ;
                    view = rhv.view ;
                    return *this ;
                }
                this_ref_t operator = ( this_rref_t rhv ) {
                    glyph_infos = std::move( rhv.glyph_infos ) ;
                    proj = rhv.proj ;
                    view = rhv.view ;
                    return *this ;
                }
            };
            natus_typedef( group_info ) ;
            natus_typedefs( natus::ntd::vector< group_info >, group_infos ) ;

            natus::concurrent::mutex_t _mtx_lis ;
            group_infos_t _gis ;

            natus_typedefs( natus::ntd::vector< size_t >, render_group_ids ) ;
            render_group_ids_t _render_groups ;

        public:

            text_render_2d( natus::ntd::string_cref_t, natus::graphics::async_views_t ) noexcept ;
            text_render_2d( this_cref_t ) = delete ;
            text_render_2d( this_rref_t ) noexcept ;
            ~text_render_2d( void_t ) noexcept ;

        public:
            
            void_t init( natus::font::glyph_atlas_res_t, size_t const = 10 ) noexcept ;

            void_t set_view_projection( natus::math::mat4f_cref_t view, natus::math::mat4f_cref_t proj ) ;
            void_t set_view_projection( size_t const, natus::math::mat4f_cref_t view, natus::math::mat4f_cref_t proj ) ;

            natus::gfx::result draw_text( size_t const group, size_t const font_id, size_t const point_size,
                natus::math::vec2f_cref_t pos, natus::math::vec4f_cref_t color, natus::ntd::string_cref_t ) ;

            //so_gfx::result set_canvas_info( canvas_info_cref_t ) ;
            natus::gfx::result prepare_for_rendering( void_t ) ;
            
            natus::gfx::result render( size_t const ) ;

            natus::gfx::result release( void_t ) ;

        private:

            bool_t need_to_render( size_t const ) const noexcept ;

        public:

            natus::ntd::string_cref_t name( void_t ) const noexcept
            {
                return _name ;
            }

        };
        natus_res_typedef( text_render_2d ) ;
    }
}
