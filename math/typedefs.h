#pragma once

#include <natus/core/types.hpp>
#include <natus/core/macros/move.h>
#include <natus/core/macros/typedef.h>
#include <natus/core/break_if.hpp>

namespace natus
{
    namespace math
    {
        using namespace natus::core::types ;

        struct x_axis { int d; };
        struct y_axis { int d; };
        struct z_axis { int d; };

        struct with_identity { int d; } ;
        struct xyzw {};
        struct yxzw {};
        struct xzyw {};

        struct axis_normalized { int d; };
    }
}
