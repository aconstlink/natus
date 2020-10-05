#pragma once

#include <natus/ntd/string.hpp>

namespace natus
{
    namespace audio
    {
        enum class backend_type
        {
            unknown,
            oal,
        };

        namespace detail
        {
            natus::ntd::string_t const __backend_type_names[] = {
                "unknown", "oal" } ;
        }

        static natus::ntd::string_cref_t to_string( natus::audio::backend_type const bt )
        {
            return detail::__backend_type_names[ size_t( bt ) ] ;
        }

        static size_t const max_backends = 1 ;
    }
}
