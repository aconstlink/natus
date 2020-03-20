#pragma once

#include "protos.h"
#include "mutex.h"

#include <natus/memory/global.h>
#include <natus/memory/allocator.hpp>

#include <natus/std/string.hpp>
#include <natus/std/map.hpp>
#include <natus/std/vector.hpp>

#include <natus/core/types.hpp>
#include <natus/core/macros/typedef.h>
#include <natus/core/macros/move.h>
#include <natus/core/boolean.h>
#include <natus/core/checks.h>

#include <thread>
#include <functional>

namespace natus
{
    namespace concurrent
    {
        using namespace natus::core::types ;

        typedef ::std::thread thread_t ;

        using memory = natus::memory::global ;

        template< typename T >
        using allocator = natus::memory::allocator<T, natus::concurrent::memory > ;

        typedef ::std::function< void_t ( void_t ) > void_funk_t ;

        namespace detail
        {
            typedef natus::std::map< natus::std::string_t, itask_ptr_t > task_map_t ;
        }

        natus_typedefs( natus::concurrent::detail::task_map_t, task_map ) ;
    }
}
