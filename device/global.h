#pragma once

#include "system.h"

namespace natus
{
    namespace device
    {
        class NATUS_DEVICE_API global
        {
            natus_this_typedefs( global ) ;


        private:

            static natus::device::system_ptr_t _sys ;

        public:

            static natus::device::system_ptr_t system( void_t ) ;
            static void_t deinit( void_t ) noexcept ;

        };
        natus_typedef( global ) ;
    }
}