#pragma once

#include "../api.h"
#include "../typedefs.h"

#include <natus/graphics/async.h>
#include <natus/graphics/variable/variable_set.hpp>
#include <natus/graphics/configuration/render_configuration.h>
#include <natus/graphics/configuration/geometry_configuration.h>

#include <natus/device/layouts/three_mouse.hpp>
#include <natus/device/layouts/ascii_keyboard.hpp>

#include <natus/memory/macros.h>
#include <imgui/imgui.h>

namespace natus
{
    namespace gfx
    {
        class NATUS_GFX_API imgui
        {
            natus_this_typedefs( imgui ) ;

        private:

            ImGuiContext* _ctx ;

            struct vertex 
            { 
                natus::math::vec2f_t pos ; 
                natus::math::vec2f_t uv ; 
                natus::math::vec4f_t color ; 
            } ;

            natus::graphics::render_configuration_res_t _rc ;
            natus::graphics::shader_configuration_res_t _sc ;
            natus::graphics::geometry_configuration_res_t _gc ;
            natus::graphics::image_configuration_res_t _ic ;

            natus::ntd::vector< natus::graphics::variable_set_res_t > _vars ;
            natus::ntd::vector< natus::graphics::render_state_sets_res_t > _render_states ;

            uint_t _width = 0 ;
            uint_t _height = 0 ;

            bool_t _texture_added = false ;

        private:


        public:

            imgui( void_t ) ;
            imgui( this_cref_t ) = delete ;
            imgui( this_rref_t ) ;
            ~imgui( void_t ) ;

            void_t init( natus::graphics::async_view_ref_t ) ;
            void_t begin( void_t ) ;
            void_t render( natus::graphics::async_view_ref_t ) ;

            typedef ::std::function< void_t ( ImGuiContext* ) > exec_funk_t ;
            void_t execute( exec_funk_t ) ;

        public:

            struct window_data
            {
                int_t width ;
                int_t height ;
            };
            natus_typedef( window_data ) ;

            void_t update( window_data_cref_t ) ;
            void_t update( natus::device::three_device_res_t ) ;
            void_t update( natus::device::ascii_device_res_t ) ;

            // Returns an id that can be used to render the passed image.
            // the image needs to be registered before via an image_configuration.
            ImTextureID texture( natus::ntd::string_in_t ) noexcept ;

        private:

            void_t do_default_imgui_init( void_t ) ;
        };
        natus_res_typedef( imgui ) ;
    }
}
