#pragma once

#include <natus/core/types.hpp>
#include <natus/core/assert.h>
#include <natus/core/macros/typedef.h>
#include <natus/core/macros/move.h>
#include <natus/core/checks.h>
#include <natus/core/break_if.hpp>

#include <string>

namespace natus
{
    namespace graphics
    {
        using namespace natus::core ;
        using namespace natus::core::types ;

        static size_t const max_backends = 2 ;
    }
}