
#pragma once

namespace natus
{
    namespace collide
    {
        enum class hit_test_type
        {
            inside,
            outside,
            intersect,
            disjoint,
            overlap,
            num_hit_test_types
        };
    }
}