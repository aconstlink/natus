#pragma once

namespace natus
{
    namespace application
    {
        enum class window_type
        {
            win32,
            x11,
            wayland
        };

        enum class render_window_type
        {
            gl,
            egl
        };
    }
}