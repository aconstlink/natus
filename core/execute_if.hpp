
#pragma once

#include "boolean.h"
#include "execute_result.h"

#include <functional>

namespace natus
{
    namespace core
    {
        using namespace natus::core::types ;

        static natus::core::execute_result execute_if( bool_t cond, std::function< natus::core::execute_result ( void_t ) > funk )
        {
            if( natus::core::is_not( cond ) )
                return natus::core::execute_result::cond_false ;

            return funk() ;
        }
    }
}


