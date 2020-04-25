
#pragma once

#include "../api.h"
#include "../protos.h"
#include "../typedefs.h"

#include "../backend/types.h"

#include <natus/std/vector.hpp>

namespace natus
{
    namespace gpu
    {
        class NATUS_GPU_API geometry_configuration
        {
            natus_this_typedefs( geometry_configuration ) ;

        };
        natus_soil_typedef( geometry_configuration ) ;
    }
}