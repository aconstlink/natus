#pragma once

namespace natus
{
    namespace math
    {
        /// missing classic euler angle sequences
        enum class euler_sequence
        {
            invalid,
            xyz,
            yzx,
            zxy,
            xzy,
            zyx,
            yxz
        };
    }
}
