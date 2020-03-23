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

        namespace natus_vector
        {
            struct x_axis { int d; };
            struct y_axis { int d; };
            struct z_axis { int d; };
        }

        namespace natus_matrix
        {
            struct with_identity { int d; } ;
            struct xyzw {};
            struct yxzw {};
            struct xzyw {};
        }

        namespace natus_quaternion
        {
            struct axis_normalized { int d; };
        }
    }
}