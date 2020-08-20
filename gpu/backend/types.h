#pragma once

namespace natus
{
    namespace gpu
    {
        enum class backend_type
        {
            unknown,
            gl3,
            es3
        };

        namespace detail
        {
            natus::ntd::string_t const __backend_type_names[] = {
                "unknown", "gl3", "es3" } ;
        }

        static natus::ntd::string_cref_t to_string( natus::gpu::backend_type const bt )
        {
            return detail::__backend_type_names[ size_t( bt ) ] ;
        }
    }
}
