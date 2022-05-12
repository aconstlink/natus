
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
        class NATUS_TOOL_API player_controller
        {
            natus_this_typedefs( player_controller ) ;

        private:

            bool_t _internal_play = false ;

            bool_t _play = false ;
            bool_t _pause = false ;

        public:

            void_t do_tool( natus::ntd::string_cref_t label, natus::tool::imgui_view_t ) noexcept ;

            bool_t is_play_clicked( void_t ) const noexcept ;
            bool_t is_pause_clicked( void_t ) const noexcept ;
            
        };
        natus_res_typedef( player_controller ) ;
    }
}