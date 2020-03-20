#pragma once

namespace natus
{
    namespace concurrent
    {
        enum result
        {
            ok,
            failed,
            invalid_argument,
            mt_processing
        };
    }
}