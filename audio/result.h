
#pragma once

#include <natus/memory/macros.h>

namespace natus
{
    namespace audio
    {
        enum class result
        {
            ok, 
            failed,
            invalid,
            invalid_argument,
            initial,
            in_progress,
            idle
        };
        natus_res_typedef( result ) ;


        enum class async_result
        {
            ok,
            failed,
            invalid,
            in_transit,
            user_edit
        };
    }
}