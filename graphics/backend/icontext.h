
#pragma once

#include "../api.h"
#include "../typedefs.h"
#include "../protos.h"

namespace natus
{
    namespace graphics
    {
        class NATUS_GRAPHICS_API icontext 
        {
        public:

            virtual ~icontext( void_t ) noexcept {}
        };
        natus_typedef( icontext ) ;
    }
}