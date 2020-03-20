#pragma once

#include <natus/memory/allocator.hpp>

#include <vector>

namespace natus
{
    namespace std
    {
        //template< typename T >
        //using vector = ::std::vector< T, natus::memory::allocator<T> > ;

        // for now, we use the default allocator
        template< typename T >
        using vector = ::std::vector< T > ;
    }
}