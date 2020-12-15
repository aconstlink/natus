#pragma once 

#include "api.h"
#include "typedefs.h"
#include "future_item.hpp"

#include <natus/property/property_sheet.hpp>
#include <natus/io/database.h>

namespace natus
{
    namespace format
    {
        class NATUS_FORMAT_API imodule
        {
        public:

            //imodule( void_t ) {}
            virtual ~imodule( void_t ) {}

            virtual natus::format::future_item_t import_from( natus::io::location_cref_t loc, 
                natus::io::database_res_t ) noexcept = 0 ;

            virtual natus::format::future_item_t import_from( natus::io::location_cref_t loc, 
                natus::io::database_res_t, natus::property::property_sheet_res_t ) noexcept = 0 ;
        };
        natus_res_typedef( imodule ) ;
    }
}