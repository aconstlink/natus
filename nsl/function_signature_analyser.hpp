

#pragma once

#include "typedefs.h"
#include "enums.hpp"


#include <natus/ntd/vector.hpp>

namespace natus
{
    namespace nsl
    {
        // @precondition the line passed in must 
        // be a complete statement
        class function_signature_analyser
        {
            natus_this_typedefs( function_signature_analyser ) ;

        private:

            natus::ntd::string_t _line ;

            struct funk
            {
                natus::nsl::type_t return_type ;
                natus::ntd::string_t name ;
                natus::ntd::vector< natus::nsl::type_t > args ;
            };
            natus_typedef( funk ) ;

            natus::ntd::vector< funk_t > _funks ;

        public:

            function_signature_analyser( natus::ntd::string_cref_t line ) noexcept : _line( line ) {}


        };
        natus_typedef( function_signature_analyser ) ;
    }
}