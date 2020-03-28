#pragma once

#include <natus/memory/global.h>
#include <natus/memory/allocator.hpp>

#include <natus/core/assert.h>
#include <natus/core/types.hpp>
#include <natus/core/macros/typedef.h>

#include <string>
#include <vector>
#include <algorithm>

namespace natus
{
    namespace gl
    {
        using namespace natus::core::types ;
        using memory = natus::memory::global ;

        template< typename T >
        using allocator = natus::memory::allocator< T, natus::gl::memory > ;
    }
}