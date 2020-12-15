#pragma once

#include "typedefs.h"

#include <future>

namespace natus
{
    namespace format
    {
        struct item
        {
            virtual ~item( void_t ) {}
        };
        natus_res_typedef( item ) ;
        typedef std::future< item_res_t > future_item_t ;

        struct status_item : public item
        {
            natus::ntd::string_t msg ;

            status_item( natus::ntd::string_cref_t msg_ ) : msg( msg_ ) {}
        };
        natus_res_typedef( status_item ) ;
        typedef std::future< status_item_res_t > future_status_t ;
    }
}