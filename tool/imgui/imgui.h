#pragma once

#include "../api.h"
#include "../typedefs.h"

#include <natus/graphics/async.h>
#include <natus/graphics/variable/variable_set.hpp>
#include <natus/graphics/object/render_object.h>
#include <natus/graphics/object/geometry_object.h>

#include <natus/device/layouts/three_mouse.hpp>
#include <natus/device/layouts/ascii_keyboard.hpp>

#include <natus/memory/macros.h>
#include <imgui/imgui.h>

namespace natus
{
    namespace tool
    {
        class NATUS_TOOL_API imgui
        {
            natus_this_typedefs( imgui ) ;

        private:

            ImGuiContext* _ctx = nullptr ;

            struct vertex 
            { 
                natus::math::vec2f_t pos ; 
                natus::math::vec2f_t uv ; 
                natus::math::vec4f_t color ; 
            } ;

            natus::graphics::render_object_res_t _rc ;
            natus::graphics::shader_object_res_t _sc ;
            natus::graphics::geometry_object_res_t _gc ;
            natus::graphics::image_object_res_t _ic ;

            natus::ntd::vector< natus::graphics::variable_set_res_t > _vars ;
            natus::graphics::state_object_res_t  _render_states ;

            uint_t _width = 0 ;
            uint_t _height = 0 ;

            bool_t _texture_added = false ;

            // we store this, so tools can have easy access to it.
            natus::graphics::async_view_t _async ;

        private:


        public:

            imgui( void_t ) ;
            imgui( this_cref_t ) = delete ;
            imgui( this_rref_t ) ;
            ~imgui( void_t ) ;

            void_t init( natus::graphics::async_view_t ) ;
            void_t begin( void_t ) noexcept ;
            void_t end( void_t ) noexcept ;
            void_t render( natus::graphics::async_view_t ) ;

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
            // the image needs to be registered before via an image_object.
            ImTextureID texture( natus::ntd::string_in_t ) noexcept ;

            natus::graphics::async_view_t async( void_t ) const noexcept ;

        private:

            void_t do_default_imgui_init( void_t ) ;
        };
        natus_res_typedef( imgui ) ;

        class imgui_view
        {
            natus_this_typedefs( imgui_view ) ;

        private:

            imgui_res_t _imres ;

        public:

            imgui_view( imgui_res_t im ) : _imres( im )
            {}

            imgui_view( this_cref_t rhv ) noexcept
            {
                _imres = rhv._imres ;
            }

            imgui_view( this_rref_t rhv ) noexcept
            {
                _imres = std::move( rhv._imres ) ;
            }

        public:

            ImTextureID texture( natus::ntd::string_in_t s ) noexcept 
            {
                return _imres->texture( s ) ;
            }

            natus::graphics::async_view_t async( void_t ) const noexcept 
            {
                return _imres->async() ;
            }

        };
        natus_typedef( imgui_view ) ;
    }
}

static ImVec2 const operator + ( ImVec2 const & lhv, ImVec2 const & rhv ) noexcept
{
    return ImVec2( lhv.x + rhv.x, lhv.y + rhv.y ) ;
}

static ImVec2 const operator - ( ImVec2 const & lhv, ImVec2 const & rhv ) noexcept
{
    return ImVec2( lhv.x - rhv.x, lhv.y - rhv.y ) ;
}
