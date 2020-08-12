
#pragma once

#include <natus/memory/macros.h>

namespace natus
{
    namespace gpu
    {
        enum class result
        {
            ok, 
            failed,
            invalid,
            invalid_argument
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