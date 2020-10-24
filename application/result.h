#pragma once

#include <natus/core/assert.h>
#include <natus/ntd/string.hpp>

namespace natus
{
    namespace application
    {
        enum class result
        {
            ok,
            failed,
            invalid_argument,
            terminate,
            no_app,
            failed_wgl,
            failed_glx,
            failed_d3d,
            failed_gfx_context_creation,
            invalid_extension,
            invalid_win32_handle,
            invalid_xlib_handle,

            num_results
        };

        namespace detail
        {
            /// not implemented yet.
            static natus::ntd::string_t const _app_result_strings[] =
            {
                "natus::application::result"
            } ;
        }

        static bool success( natus::application::result res )
        {
            // natus_assert( res < num_results ) ;
            return res == natus::application::result::ok ;
        }

        static bool no_success( natus::application::result res )
        {
            return !success( res );
        }

        static natus::ntd::string_cref_t to_string( natus::application::result /*res*/ )
        {
            return natus::application::detail::_app_result_strings[ 0 ] ;
        }
    }
}
