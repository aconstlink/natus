#pragma once

#include <string>

namespace natus
{
    namespace log
    {
        enum class log_level
        {
            raw,
            status,
            warning,
            error,
            critical,
            timing
        };

        namespace natus_internal
        {
            static ::std::string const __log_level_strings[] = {
                "raw", "status", "warning", "error", "critical", "timing"
            } ;
            static ::std::string const __log_level_short_strings[] = {
                "", "[s]", "[w]", "[e]", "[c]", "[t]"
            } ;
        }

        static ::std::string const& to_string( log_level level )
        {
            return natus_internal::__log_level_strings[ size_t( level ) ] ;
        }

        static ::std::string const& to_string_short( log_level level )
        {
            return natus_internal::__log_level_short_strings[ size_t( level ) ] ;
        }
    }
}
