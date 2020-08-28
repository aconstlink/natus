#pragma once

#include "api.h"
#include "typedefs.h"

#include "imodule.h"

namespace natus
{
    namespace format
    {
        class NATUS_FORMAT_API imodule_factory
        {
        public:

            virtual imodule_res_t create_module( natus::ntd::string_cref_t ext ) noexcept = 0 ;
        };
        natus_res_typedef( imodule_factory ) ;

        template< typename T >
        class factory : public imodule_factory
        {

        };
    }
}