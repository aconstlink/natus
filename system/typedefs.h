#pragma once

#include <natus/core/types.hpp>
#include <natus/core/assert.h>
#include <natus/core/macros/typedef.h>
#include <natus/core/macros/move.h>
#include <natus/core/checks.h>
#include <natus/core/break_if.hpp>
#include <natus/core/boolean.h>

namespace natus
{
    namespace system
    {
        using namespace natus::core::types ;

        enum class os_type
        {
            unknown,
            windows,
            linux,
            android,
            macos
        };

        enum class arch_type
        {
            unknown,
            x86_64,
            arm_64
        };

        enum class window_type
        {
            unknown,
            win32,
            x11,
            wayland,
            wgl,
            glx,
            egl
        };
        
        enum class instruction_set_type
        {
            unknown,
            sse3,
            sse4,
            altivec
        };

        enum class graphics_type
        {
            directx,
            opengl,
            opengles,
            vulkan
        };
    }
}