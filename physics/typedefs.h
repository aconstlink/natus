#pragma once

#include <natus/core/types.hpp>
#include <natus/core/assert.h>
#include <natus/core/macros/typedef.h>
#include <natus/core/macros/move.h>
#include <natus/core/checks.h>
#include <natus/core/break_if.hpp>
#include <natus/memory/macros.h>

#include <string>

#define NATUS_PHYSICS_USE_PARALLEL_FOR 1 

namespace natus
{
    namespace physics
    {
        using namespace natus::core ;
        using namespace natus::core::types ;

        enum class spawn_location_type
        {
            area,
            border
        } ;

        enum class spawn_distribution_type
        {
            uniform,
            random
        };

        enum class variation_type
        {
            fixed,
            random
        };
    }
}