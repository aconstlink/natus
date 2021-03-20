#pragma once

#include "../typedefs.h"
#include "../range_1d.hpp"

#include <functional>

namespace natus
{
    namespace concurrent
    {
        template< typename T >
        using funk_t = ::std::function< void_t ( natus::concurrent::range_1d<T> const& ) > ;
    }
}
