#pragma once

#include <natus/core/assert.h>
#include <natus/std/string.hpp>

namespace natus
{
    namespace application
    {
        enum class result
        {
            ok,
            failed,
            terminate,
            not_ready,
            failed_gfx_context_creation,
            failed_gfx_context_release,
            failed_render_thread_creation,
            failed_wgl,
            failed_glx_version,
            invalid_win32_handle,
            invalid_window_handle,
            invalid_window_name,
            invalid_argument,
            invalid_gfx_api_version,
            invalid_extension,
            thread_running,
            thread_stoped,
            not_implemented,
            no_decorated,
            micro_activate,
            micro_deactivate,
            micro_background,
            micro_switch,
            num_results
        };

        namespace detail
        {
            /// not implemented yet.
            static natus::std::string_t const _app_result_strings[] =
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

        static natus::std::string_cref_t to_string( natus::application::result /*res*/ )
        {
            return natus::application::detail::_app_result_strings[ 0 ] ;
        }
    }
}
