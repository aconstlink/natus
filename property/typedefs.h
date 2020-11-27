#pragma once

#include <natus/ntd/vector.hpp>
#include <natus/memory/macros.h>
#include <natus/core/types.hpp>
#include <natus/core/assert.h>
#include <natus/core/macros/typedef.h>
#include <natus/core/macros/move.h>
#include <natus/core/checks.h>
#include <natus/core/break_if.hpp>

#include <string>

namespace natus
{
    namespace property
    {
        using namespace natus::core ;
        using namespace natus::core::types ;


        natus_typedefs( uint32_t, utf32 ) ;
        natus_typedefs( natus::ntd::vector< utf32_t >, code_points ) ;
    }
}