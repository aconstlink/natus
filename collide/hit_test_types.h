
#pragma once

namespace natus
{
    namespace collide
    {
        enum class hit_test_type
        {
            unknown,
            inside,
            outside,
            intersect,
            disjoint,
            overlap,
            num_hit_test_types
        };

        static natus::ntd::string_cref_t to_string( hit_test_type const ht ) noexcept
        {
            static natus::ntd::string_t __[] = { "unknown", "inside", "outside", "intersect", "disjoint", "overlap" };
            size_t const idx = size_t(ht) >= size_t(hit_test_type::num_hit_test_types) ? 0 : size_t(ht) ;
            return __[idx] ;
        }
    }
}