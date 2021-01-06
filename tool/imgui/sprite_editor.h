#pragma once

#include "imgui.h"

#include <natus/graphics/object/image_object.h>
#include <natus/format/future_item.hpp>
#include <natus/io/database.h>
#include <natus/ntd/string.hpp>
#include <natus/ntd/vector.hpp>

namespace natus
{
    namespace tool
    {
        class NATUS_TOOL_API sprite_editor
        {
            natus_this_typedefs( sprite_editor ) ;

      
        private:

            natus::io::database_res_t _db ;

            struct sprite_sheet
            {
                natus::ntd::string_t dname ;
                natus::ntd::string_t name ;
                natus::io::location_t img_loc ;
                natus::graphics::image_object_res_t img ;

                // image dimensions
                natus::math::vec2ui_t dims ;

                // pixel bounds
                natus::math::vec4ui_t bounds ;

                // animation sequences
                // boxes
                natus::math::vec4f_t region ;


                // origin in world space
                // the world space is where the window rect and 
                // the image rect life together in a 1:1 relation
                natus::math::vec2f_t origin ;

                // zoom of the original 1:1 world rect
                // zoom == 1 means the window pixel size 
                // match image pixel size => 1:1
                float_t zoom = -0.5f ;
                
            };
            natus_typedef( sprite_sheet ) ;

            natus::ntd::vector< sprite_sheet_t > _sprite_sheets ;
            int_t _cur_item = 0 ;

            int_t _pixel_ratio ;
            natus::math::vec2i_t _cur_pixel ;

            bool_t _mouse_down_rect = false ;
            natus::math::vec4ui_t _cur_rect ;
            natus::math::vec2f_t _screen_pos_image ;
            natus::math::vec2f_t _cur_mouse ;

            struct load_item
            {
                natus::ntd::string_t disp_name ;
                natus::ntd::string_t name ;
                natus::io::location_t loc ;
                natus::format::future_item_t fitem ;
            } ;
            natus_typedef( load_item ) ;

            natus::ntd::vector< load_item_t > _loads ;

        public:

            sprite_editor( void_t ) noexcept ;
            sprite_editor( natus::io::database_res_t ) noexcept ;
            sprite_editor( this_cref_t ) = delete ;
            sprite_editor( this_rref_t ) noexcept ;
            ~sprite_editor( void_t ) noexcept ;

            this_ref_t operator = ( this_cref_t ) = delete ;
            this_ref_t operator = ( this_rref_t ) noexcept ;

            void_t add_sprite_sheet( natus::ntd::string_cref_t name, 
                natus::io::location_cref_t loc ) noexcept ;

            void_t render( natus::tool::imgui_view_t ) noexcept ;

        private:

            void_t handle_mouse( natus::tool::imgui_view_t imgui, int_t const selected ) ;
            void_t show_image( natus::tool::imgui_view_t imgui, int_t const selected ) ;

            bool_t handle_rect( natus::tool::imgui_view_t imgui, natus::math::vec4ui_ref_t ) ;
            natus::math::vec4ui_t rearrange_mouse_rect( natus::math::vec4ui_cref_t ) const ;

            natus::math::vec4f_t image_rect_to_window_rect( int_t const selection,  natus::math::vec4ui_cref_t ) const ;

            natus::math::vec4f_t compute_cur_view_rect( int_t const selection ) const ;

            
        };
        natus_res_typedef( sprite_editor ) ;
    }
}