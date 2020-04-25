#pragma once

#include "protos.h"
#include "mutex.hpp"

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
#include <mutex>

namespace natus
{
    namespace concurrent
    {
        using namespace natus::core::types ;

        natus_typedefs( ::std::thread, thread ) ;
        natus_typedefs( ::std::mutex, mutex ) ;
        natus_typedefs( ::std::unique_lock<mutex_t>, ulock ) ;

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
