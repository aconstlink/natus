#pragma once

#include "imgui.h"

#include <natus/graphics/object/image_object.h>
#include <natus/format/future_item.hpp>
#include <natus/io/database.h>
#include <natus/ntd/string.hpp>
#include <natus/ntd/vector.hpp>

#include <array>

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
                // format : x0, y0, x1, y1
                natus::ntd::vector< natus::math::vec4ui_t > bounds ;

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
            natus::math::vec2i_t _cr_mouse ;

            // content region dimensions
            natus::math::vec2f_t _crdims ;

            // offset from window orig to content region
            natus::math::vec2ui_t _croff ;

            bool_t _mouse_down_drag = false ;
            natus::math::vec2ui_t _drag_begin ;

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

            // show the selected image in the content region
            void_t show_image( natus::tool::imgui_view_t imgui, int_t const selected ) ;

            // handle the rect the user draws with the mouse
            bool_t handle_rect( natus::tool::imgui_view_t imgui, natus::math::vec4ui_ref_t ) ;

            // rearrange rect so that 
            // smallest xy is bottom/left and 
            // largest xy is top/right
            // all image rects are stored like this
            // format: ( x0, y0, x1, y1 )
            natus::math::vec4ui_t rearrange_mouse_rect( natus::math::vec4ui_cref_t ) const ;

            // returns the current rect that is mapped to the content region in world space.
            // format: ( x0, y0, x1, y1 )
            natus::math::vec4f_t compute_cur_view_rect( int_t const selection ) const ;

            // image rect is bottom/left -> top/right with format x0,y0, x1,y1
            // window rect is top/left -> bottom/right with format x, y, w, h
            // @note the imgui window coords are 
            // x > 0 => left to right
            // y > 0 => top to bottom
            natus::math::vec4f_t image_rect_to_window_rect( int_t const selection, 
                natus::math::vec4f_cref_t image_rect ) const ;

            bool_t is_window_rect_inside_content_region( natus::math::vec4f_ref_t rect, bool_t const fix_coords = true ) const ;

            // rect in window coords with format x, y, w, h
            void_t draw_rect( natus::math::vec4f_cref_t, natus::math::vec4ui_cref_t = 
                natus::math::vec4ui_t(255) ) ;
            void_t draw_rect_for_scale( natus::math::vec4f_cref_t, natus::math::vec4ui_cref_t = 
                natus::math::vec4ui_t(255) ) ;

            void_t draw_scales( natus::math::vec4f_cref_t, natus::math::vec4ui_cref_t prect, natus::math::vec4ui_t = 
                natus::math::vec4ui_t(255) ) ;

            bool_t is_ip_mouse_in_bound( natus::math::vec4ui_cref_t ) const ;

            // @precondition mouse must be in bound
            // @param rect image space rect with format x0,y0,x1,y1
            // @param hit
            // [0] : bottom left
            // [1] : left
            // [2] : top left
            // [3] : top
            // [4] : top right
            // [5] : right
            // [6] : bottom right
            // [7] : bottom
            bool_t intersect_bound_location( natus::math::vec4ui_cref_t rect, std::array< bool_t, 8 > & hit ) const ; 
            
        };
        natus_res_typedef( sprite_editor ) ;
    }
}