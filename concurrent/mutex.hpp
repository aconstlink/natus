#pragma once

#include <mutex>
#include <condition_variable>

namespace natus
{
    namespace concurrent
    {
        typedef ::std::mutex mutex_t ;
        typedef ::std::lock_guard< mutex_t > lock_guard_t ;
        typedef ::std::unique_lock< mutex_t > lock_t ;
        typedef ::std::condition_variable condition_variable_t ;
    }
}