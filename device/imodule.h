#pragma once

#include "api.h"
#include "typedefs.h"
#include "device.hpp"

namespace natus
{
    namespace device
    {
        class NATUS_DEVICE_API imodule
        {

        public:

            typedef ::std::function< void_t ( natus::device::idevice_res_t ) > search_funk_t ;
            virtual void_t search( search_funk_t ) = 0 ;
            virtual void_t update( void_t ) = 0 ;
        };
        natus_res_typedef( imodule ) ;
    }
}