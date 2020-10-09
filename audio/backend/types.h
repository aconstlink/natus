#pragma once

#include <natus/ntd/string.hpp>

namespace natus
{
    namespace audio
    {
        enum class backend_type
        {
            unknown,
            openal,
        };

        namespace detail
        {
            natus::ntd::string_t const __backend_type_names[] = {
                "unknown", "openal" } ;
        }

        static natus::ntd::string_cref_t to_string( natus::audio::backend_type const bt )
        {
            return detail::__backend_type_names[ size_t( bt ) ] ;
        }

        /// including unknown
        static size_t const max_backends = 2 ;
    }
}
