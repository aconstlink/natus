
#pragma once

#include "../typedefs.h"

namespace natus
{
    namespace device
    {
        namespace layouts
        {
            class ascii
            {
            public:

                enum class input_component
                {
                    num_components
                };

                enum class output_component
                {
                    num_components
                };
            };
            natus_typedef( ascii ) ;
        }
        natus_soil_typedefs( natus::device::device< natus::device::layouts::ascii_t>, ascii_device ) ;
    }
}