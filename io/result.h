#pragma once

#include <natus/ntd/string.hpp>

namespace natus
{
    namespace io
    {
        enum class result
        {
            ok,
            failed,
            processing,
            state_change_failed,
            file_does_not_exist,
            invalid,
            invalid_argument,
            unknown,
            invalid_handle ,
            num_results
        };

        static bool success( result r ) { return r == natus::io::result::ok ; }
        static bool no_success( result r ) { return !success( r ) ; }

        namespace natus_internal
        {
            static natus::ntd::string_t const __result_strings[] = {
                "ok", "failed", "processing", "state_change_failed",
                "file_does_not_exist", "invalid", "invalid_argument", "unknown", "invalid_handle"
            } ;
        }

        static natus::ntd::string_cref_t to_string( natus::io::result r )
        {
            return size_t( r ) < size_t( natus::io::result::num_results ) ?
                natus_internal::__result_strings[ size_t( r ) ] :
                natus_internal::__result_strings[ size_t( natus::io::result::num_results ) - 1 ] ;
        }
    }
}