

#pragma once

#include "imgui.h"

namespace natus
{
    namespace tool
    {
        struct NATUS_TOOL_API custom_imgui_widgets
        {
            static bool knob(const char* label, float* p_value, float v_min, float v_max) noexcept ;

            static bool_t direction( char const * label, natus::math::vec2f_ref_t dir ) noexcept ;
        };
    }
}