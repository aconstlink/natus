
#pragma once

#include "../../imodule.h"

namespace natus
{
    namespace device
    {
        namespace win32
        {
            class NATUS_DEVICE_API rawinput_module : public imodule
            {
                natus_this_typedefs( rawinput_module ) ;
            };
            natus_soil_typedef( rawinput_module ) ;
        }
    }
}