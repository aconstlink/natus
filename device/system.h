#pragma once

#include "api.h"
#include "typedefs.h"
#include "imodule.h"

namespace natus
{
    namespace device
    {
        class NATUS_DEVICE_API system
        {
            natus_this_typedefs( system ) ;

        private:

            natus::std::vector< imodule_res_t > _modules ;

        public:

            system( void_t ) ;
            system( this_cref_t ) = delete ;
            system( this_rref_t ) ;
            ~system( void_t ) ;

        public:

            void_t add_module( natus::device::imodule_res_t ) ;

            void_t search( natus::device::imodule::search_funk_t ) ;
            void_t update( void_t ) ;
        };
        natus_soil_typedef( system ) ;
    }
}