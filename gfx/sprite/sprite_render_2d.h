#pragma once

#include "../api.h"
#include "../typedefs.h"

#include <natus/graphics/async.h>
#include <natus/graphics/object/state_object.h>
#include <natus/graphics/object/array_object.h>
#include <natus/graphics/object/render_object.h>
#include <natus/graphics/object/geometry_object.h>
#include <natus/graphics/variable/variable_set.hpp>

#include <natus/ntd/vector.hpp>

#include <natus/math/matrix/matrix2.hpp>

namespace natus
{
    namespace gfx
    {
        class NATUS_GFX_API sprite_render_2d
        {
            natus_this_typedefs( sprite_render_2d ) ;

        private: // data
            
            struct sprite
            {
                natus_this_typedefs( sprite ) ;

                natus::math::vec2f_t pos ;
                natus::math::mat2f_t frame ;
                natus::math::vec2f_t scale ;
                natus::math::vec4f_t uv_rect ;
                size_t slot = 0 ;
            };
            natus_typedef( sprite ) ;

            struct layer
            {
                natus::ntd::vector< sprite_t > sprites ;
            };
            natus_typedef( layer ) ;

            natus::ntd::vector< layer_t > _layers ;

            size_t _num_sprites = 0 ;

        private: // graphics

            struct per_layer_render_data
            {
                size_t num_elems = 0 ;
                size_t num_quads = 0 ;
            };
            natus::ntd::vector< per_layer_render_data > _render_data ;

            struct render_layer_info
            {
                size_t start = 0 ;
                size_t end = 0 ;
            };
            natus::ntd::vector< render_layer_info > _render_layer_infos ;

            struct vertex
            {
                natus::math::vec2f_t pos ;
            };

            struct the_data
            {
                // world pos, scale : ( x, y, sx, sy )
                natus::math::vec4f_t pos_scale ;
                // coords frame (X, Y)
                natus::math::vec4f_t frame ;
                // uv rect, bottom left (x0,y0,x1,y1)
                natus::math::vec4f_t uv_rect ;

                // (slot, free, free, free )
                natus::math::vec4f_t additional_info ;

                // animate the uv coords in x and y direction
                // (x ani, y ani, free, free)
                natus::math::vec4f_t uv_anim ;

                

                // color the sprite
                //natus::math::vec4f_t color ;
            };

            natus::ntd::string_t _name ;
            natus::graphics::async_views_t _asyncs ;
            natus::graphics::state_object_res_t _rs ;
            natus::graphics::array_object_res_t _ao ;
            natus::graphics::shader_object_res_t _so ;
            natus::graphics::render_object_res_t _ro ;
            natus::graphics::geometry_object_res_t _go ;
            
            size_t _max_quads = 2000 ;

            void_t add_variable_set( natus::graphics::render_object_ref_t ) noexcept ;

            bool_t _image_name_changed = true ;
            natus::ntd::string_t _image_name ;

        public:

            #if 0
            struct sprite_sheet
            {
                struct sprite
                {
                    natus::math::vec4f_t uv_rect ;
                };

                // animaton times in milliseconds
                struct animation
                {
                    
                    size_t duration ;
                };

                // connects a sequence of sprites with animation data
                struct sprite_sequence
                {
                    natus::ntd::string_t name ;
                    natus::ntd::vector< sprite > sprites ;
                    natus::ntd::vector< size_t > durations ;
                    // animation type (single, repreat, reverse, ...)
                    
                };

                natus::ntd::vector< sprite_sequence > sequences ;
            };
            #endif

        public:

            sprite_render_2d( void_t ) ;
            sprite_render_2d( this_cref_t ) = delete ;
            sprite_render_2d( this_rref_t ) ;
            ~sprite_render_2d( void_t ) ;

            void_t init( natus::ntd::string_cref_t, natus::ntd::string_cref_t, natus::graphics::async_views_t ) noexcept ;
            void_t release( void_t ) noexcept ;

        public:

            void_t set_texture( natus::ntd::string_cref_t name ) noexcept ;

            // draw a sprite/uv rect from a texture of the screen
            void_t draw( size_t const l, natus::math::vec2f_cref_t pos, natus::math::mat2f_cref_t frame, natus::math::vec2f_cref_t scale, natus::math::vec4f_cref_t uv_rect, size_t const slot ) noexcept ;

        public:

            // copy all data to the gpu buffer and transmit the data
            void_t prepare_for_rendering( void_t ) noexcept ;
            void_t render( size_t const ) noexcept ;
        };
        natus_res_typedef( sprite_render_2d ) ;
    }
}