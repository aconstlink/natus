

#pragma once

#include "../typedefs.h"

namespace natus
{
    namespace audio
    {
        class ibuffer
        {
        public: 
            virtual ~ibuffer( void_t ) {}
        };
        natus_res_typedef( ibuffer ) ;
    }
}