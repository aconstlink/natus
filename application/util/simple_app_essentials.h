#pragma once

#include "../app.h"

#include <natus/device/global.h>
#include <natus/gfx/camera/pinhole_camera.h>
#include <natus/gfx/primitive/primitive_render_2d.h> 
#include <natus/gfx/font/text_render_2d.h>
#include <natus/gfx/primitive/line_render_3d.h>
#include <natus/gfx/primitive/primitive_render_3d.h>


namespace natus
{
    namespace application
    {
        namespace util
        {
            using namespace natus::core::types ;

            class NATUS_APPLICATION_API simple_app_essentials
            {
                natus_this_typedefs( simple_app_essentials ) ;

            private:

                natus::graphics::async_views_t _graphics ;

                natus::gfx::text_render_2d_res_t _tr ;
                natus::gfx::primitive_render_2d_res_t _pr ;
                natus::gfx::line_render_3d_res_t _lr3 ;
                natus::gfx::primitive_render_3d_res_t _pr3 ;

                natus::math::vec2f_t _window_dims = natus::math::vec2f_t( 1.0f ) ;

                bool_t _do_tool = true ;

                natus::io::database_res_t _db ;

                bool_t _has_font = false ;

            private: // device 

                natus::device::three_device_res_t _dev_mouse ;
                natus::device::ascii_device_res_t _dev_ascii ;
                natus::math::vec2f_t _cur_mouse ;

            private: // camera

                natus::gfx::pinhole_camera_t _camera_0 ;
                float_t _far = 1000.0f ;
                float_t _near = 0.1f ;

            public:

                simple_app_essentials( void_t ) noexcept ;
                simple_app_essentials( this_cref_t ) = delete ;
                simple_app_essentials( this_rref_t rhv ) noexcept ;
                ~simple_app_essentials( void_t ) noexcept ;

                this_ref_t operator = ( this_rref_t ) noexcept ;

            public:

                void_t init_graphics( natus::ntd::string_cref_t, natus::graphics::async_views_t ) noexcept ;
                void_t init_font( void_t ) noexcept ;
                void_t init_data( natus::io::path_cref_t base, natus::io::path_cref_t rel, natus::io::path_cref_t name ) noexcept ;
                void_t init_device( void_t ) noexcept ;

            public:

                void_t on_event( natus::application::app::window_id_t const, natus::application::app::window_event_info_in_t wei ) noexcept ;
                void_t on_device( natus::application::app::device_data_in_t ) noexcept ;
                void_t on_graphics_begin( natus::application::app_t::render_data_in_t ) noexcept ;
                void_t on_graphics_end( size_t const num_layers ) noexcept ;
                void_t on_tool( natus::application::app::tool_data_ref_t ) noexcept ;
            };
            natus_res_typedef( simple_app_essentials ) ;
        }
    }
}