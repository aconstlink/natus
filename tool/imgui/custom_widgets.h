

#pragma once

#include "imgui.h"

namespace natus
{
    namespace tool
    {
        struct NATUS_TOOL_API custom_imgui_widgets
        {
            natus_this_typedefs( custom_imgui_widgets ) ;

            static bool knob(const char* label, float* p_value, float v_min, float v_max) noexcept ;

            static bool_t direction( char const * label, natus::math::vec2f_ref_t dir ) noexcept ;

            static void_t text_overlay( char const * label, natus::ntd::string_cref_t text, int corner = 0 ) noexcept ;

            static bool_t overlay_begin( char const * label, int corner = 0 ) noexcept ;
            static void_t overlay_end( void_t ) noexcept ;
        };
    }
}