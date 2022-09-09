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

            // provides the user with 
            // - some renderers for quick primitive rendering
            // - basic 2d text renderer
            // - default camera
            // - camera movement with mouse and keyboard
            // - database
            // - font loading on default data path
            // - mouse coords on screen
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

                natus::io::database_res_t _db ;

                bool_t _has_font = false ;

            private: // device 

                natus::device::three_device_res_t _dev_mouse ;
                natus::device::ascii_device_res_t _dev_ascii ;
                natus::math::vec2f_t _cur_mouse ;
                natus::math::vec2f_t _cur_mouse_nrm ;

                bool_t _left_down = false ;
                bool_t _right_down = false ;
                bool_t _do_tool = false ;

            private: // camera

                natus::gfx::pinhole_camera_res_t _camera_0 ;
                float_t _far = 1000.0f ;
                float_t _near = 0.1f ;

            public:

                simple_app_essentials( void_t ) noexcept ;
                simple_app_essentials( this_cref_t ) = delete ;
                simple_app_essentials( this_rref_t rhv ) noexcept ;
                ~simple_app_essentials( void_t ) noexcept ;

                this_ref_t operator = ( this_rref_t ) noexcept ;

            public:

                struct init_struct
                {
                    struct init_graphics
                    {
                        natus::ntd::string_t app_name ;
                        natus::graphics::async_views_t graphics ;
                    };
                    
                    struct init_database
                    {
                        natus::io::path_t base ;
                        natus::io::path_t rel; 
                        natus::io::path_t name ;
                    };

                    init_graphics ig ;
                    init_database idb ;
                };
                natus_typedef( init_struct ) ;

                void_t init( init_struct_cref_t ) noexcept ;

            private:

                void_t init_graphics( natus::ntd::string_cref_t, natus::graphics::async_views_t ) noexcept ;
                void_t init_font( void_t ) noexcept ;
                void_t init_database( natus::io::path_cref_t base, natus::io::path_cref_t rel, natus::io::path_cref_t name ) noexcept ;
                void_t init_device( void_t ) noexcept ;

            public:

                void_t on_event( natus::application::app::window_id_t const, natus::application::app::window_event_info_in_t wei ) noexcept ;
                void_t on_device( natus::application::app::device_data_in_t ) noexcept ;
                void_t on_graphics_begin( natus::application::app_t::render_data_in_t ) noexcept ;
                void_t on_graphics_end( size_t const num_layers ) noexcept ;
                void_t on_tool( natus::application::app::tool_data_ref_t ) noexcept ;
                void_t on_shutdown( void_t ) noexcept ;

            public:

                natus::gfx::text_render_2d_res_t get_text_render( void_t ) noexcept { return _tr ; }
                natus::gfx::primitive_render_2d_res_t get_prim_render( void_t ) noexcept { return _pr ; }
                natus::gfx::line_render_3d_res_t get_line_render_3d( void_t ) noexcept { return _lr3 ; }
                natus::gfx::primitive_render_3d_res_t get_prim_render_3d( void_t ) noexcept { return _pr3 ; }

                natus::gfx::pinhole_camera_res_t get_camera_0( void_t ) noexcept { return _camera_0 ; }

                natus::math::vec2f_t get_cur_mouse_pos( void_t ) const noexcept { return _cur_mouse ; }
                natus::math::vec2f_t get_cur_mouse_pos_nrm( void_t ) const noexcept { return _cur_mouse_nrm ; }
                
                bool_t left_down( void_t ) const noexcept { return _left_down ; }
                bool_t right_down( void_t ) const noexcept { return _right_down ; }
                natus::math::vec2f_t get_window_dims( void_t ) const noexcept { return _window_dims ; }

                bool_t do_tool( void_t ) const noexcept { return _do_tool ; }
                void_t set_do_tool( bool_t const b ) noexcept { _do_tool = b ; }

                natus::io::database_res_t get_database( void_t ) noexcept { return _db ; }
                natus::io::database_res_t db( void_t ) noexcept { return _db ; }

            };
            natus_res_typedef( simple_app_essentials ) ;
        }
    }
}