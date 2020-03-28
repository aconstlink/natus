#pragma once

#include "typedefs.h"
#include <natus/std/string.hpp>

namespace natus
{
    namespace gl
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
            static const natus::std::string_t __graphics_result_strings[] = {
                "natus::gl::result"
            } ;
        }

        /// not implemented yet.
        static natus::std::string_cref_t to_string( natus::gl::result /*res*/ )
        {
            return natus::gl::detail::__graphics_result_strings[ 0 ] ;
        }

        static bool_t success( natus::gl::result res )
        {
            return res == natus::gl::result::ok ;
        }

        static bool_t no_success( natus::gl::result res )
        {
            return !success( res ) ;
        }
    }
}