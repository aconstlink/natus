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
                natus::ntd::string_t name ;
                natus::io::location_t img_loc ;
                natus::graphics::image_object_res_t img ;

                natus::math::vec2ui_t dims ;

                // animation sequences
                // boxes
                
                
            };
            natus_typedef( sprite_sheet ) ;

            natus::ntd::vector< sprite_sheet_t > _sprite_sheets ;


            struct load_item
            {
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

        };
        natus_res_typedef( sprite_editor ) ;
    }
}