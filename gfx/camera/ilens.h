#pragma once

#include "../typedefs.h"
#include "../protos.h"
#include "../api.h"

#include <natus/math/vector/vector3.hpp>
#include <natus/math/matrix/matrix3.hpp>
#include <natus/math/matrix/matrix4.hpp>
#include <natus/math/utility/3d/transformation.hpp>

namespace natus
{
    namespace gfx
    {
        class NATUS_GFX_API ilens
        {
        public:

        };
        natus_res_typedef( ilens ) ;
        typedef natus::memory::res_t< ilens_t > lens_res_t ;
    }
}