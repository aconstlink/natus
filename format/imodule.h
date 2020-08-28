#pragma once 

#include "api.h"
#include "typedefs.h"

#include <natus/io/database.h>

namespace natus
{
    namespace format
    {
        class NATUS_FORMAT_API imodule
        {
        public:

            imodule( void_t ) {}
            virtual ~imodule( void_t ) {}

            virtual natus::ntd::vector< natus::ntd::string_t > const &  supported_formats( void_t ) const noexcept = 0 ;
            virtual bool_t import( natus::ntd::string_cref_t loc, natus::io::database_res_t ) noexcept = 0 ;
        };
        natus_res_typedef( imodule ) ;
    }
}