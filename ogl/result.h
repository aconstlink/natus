#pragma once

#include "typedefs.h"
#include <natus/ntd/string.hpp>

namespace natus
{
    namespace ogl
    {
        enum class result
        {
            ok,
            failed,
            failed_load_function,
            num_results
        };

        typedef result* result_ptr_t ;

        namespace detail
        {
            static const natus::ntd::string_t __graphics_result_strings[] = {
                "natus::ogl::result"
            } ;
        }

        /// not implemented yet.
        static natus::ntd::string_cref_t to_string( natus::ogl::result /*res*/ )
        {
            return natus::ogl::detail::__graphics_result_strings[ 0 ] ;
        }

        static bool_t success( natus::ogl::result res )
        {
            return res == natus::ogl::result::ok ;
        }

        static bool_t no_success( natus::ogl::result res )
        {
            return !success( res ) ;
        }
    }
}