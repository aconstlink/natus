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
    namespace log
    {
        using namespace natus::core ;
        using namespace natus::core::types ;

        natus_typedefs( ::std::string, string ) ;
    }
}